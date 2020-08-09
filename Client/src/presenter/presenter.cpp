#include "presenter.h"
#include <string.h>
#include "opencv.hpp"

#include "../model/IPResolver.h"
#include "../camera/CameraFactory.h"



Presenter::Presenter(IView& view, Tracker* tracker, ConfigMgr* conf_mgr)
{
	this->conf_mgr = conf_mgr;
	ConfigData prefs = conf_mgr->getConfig();
	this->view = &view;
	this->view->connect_presenter(this);
	this->paint = prefs.show_video_feed;
	sync_ui_inputs();

	this->t = tracker;


	CameraFactory camfactory;
	camera = camfactory.buildCamera();

	if (!camera->is_valid)
	{
		std::cout << "[ERROR] NO CAMERAS AVAILABLE" << std::endl;
		this->view->set_enabled(false);
		this->view->show_message("No cameras detected. Plug one and restart the program.", MSG_SEVERITY::CRITICAL);
	}
	else
	{
		// Request sockets (UDP Sender) only if needed.
		std::string ip_str = prefs.ip;
		int port = prefs.port;
		init_sender(ip_str, port);
	}
	

}

Presenter::~Presenter()
{
	delete this->udp_sender; 
	delete this->camera;
	delete this->t;
}


void Presenter::init_sender(std::string &ip, int port)
{
	// Updata only if needed.
	if (this->udp_sender != NULL)
		if (ip != this->udp_sender->ip && port != this->udp_sender->port)
			return;
	
	if (this->udp_sender != NULL)
		delete(this->udp_sender);

	std::string ip_str = ip;
	int port_dest = port;
	if (QString(ip_str.data()).simplified().replace(" ", "").size() < 2)
		ip_str = network::get_local_ip();

	if (port_dest == 0)
		port_dest = 4242;

	this->udp_sender = new UDPSender(ip_str.data(), port_dest);
}


void Presenter::run_loop()
{
	FaceData d = FaceData();

	int video_frame_buff_size = camera->width * camera->height * 3;
	uint8_t *video_tex_pixels = new uint8_t[video_frame_buff_size];


	cv::Scalar color_blue(255, 0, 0);
	cv::Scalar color_magenta(255, 0, 255);

	double buffer_data[6];

	camera->start_camera();


	while(run)
	{
		camera->get_frame(video_tex_pixels);
		cv::Mat mat(camera->height, camera->width, CV_8UC3, video_tex_pixels);

		t->predict(mat, d);

		if (d.face_detected)
		{
			if (paint)
			{
				// Paint landmarks
				for (int i = 0; i < 66; i++)
				{
					cv::Point p(d.landmark_coords[2 * i + 1], d.landmark_coords[2 * i]);
					cv::circle(mat, p, 2, color_magenta, 3);
				}
				cv::Point p1(d.face_coords[0], d.face_coords[1]);
				cv::Point p2(d.face_coords[2], d.face_coords[3]);
				cv::rectangle(mat, p1, p2, color_blue, 1);
			}
			

			send_data(buffer_data, d);
		}

		if (paint)
		{
			cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
			view->paint_video_frame(mat);
		}

		cv::waitKey(35);
	}

	camera->stop_camera();
	delete[] video_tex_pixels;
}


void Presenter::send_data(double* buffer_data, FaceData& facedata)
{
	//Send data
	buffer_data[0] = facedata.translation[0] * 10;
	buffer_data[1] = facedata.translation[1] * 10;
	buffer_data[2] = facedata.translation[2] * 10;
	buffer_data[3] = facedata.rotation[1];   // Yaw
	buffer_data[4] = facedata.rotation[0];   //Pitch
	buffer_data[5] = facedata.rotation[2];   //Roll
	udp_sender->send_data(buffer_data);
}


void Presenter::toggle_tracking()
{
	run = !run;

	ConfigData curr_config = this->conf_mgr->getConfig();
	view->set_tracking_mode(run);

	if (run)
		run_loop();
}

void Presenter::save_prefs(const ConfigData& data)
{
	conf_mgr->updateConfig(data);

	// Update position solver priors
	this->t->solver->set_prior_pitch(data.prior_pitch);
	this->t->solver->set_prior_yaw(data.prior_yaw);
	this->t->solver->set_prior_distance(data.prior_distance);

	// Reset UDPSender
	std::string ip_str = data.ip;
	int port = data.port;
	init_sender(ip_str, port);

	// Disable painting parts from the run loop if needed
	this->paint = data.show_video_feed;

	sync_ui_inputs();
}


void Presenter::sync_ui_inputs()
{
	this->view->update_view_state(conf_mgr->getConfig());
}


void Presenter::close_program()
{
	//Assure we stop tracking loop.
	run = false;
	// Assure the camera is released (some cameras have a "recording LED" which can be annoying to have on)
	camera->stop_camera();
	// The remaining resources will be released on destructor.
}