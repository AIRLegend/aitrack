#include "presenter.h"
#include <string.h>
#include "opencv.hpp"

#include "../model/IPResolver.h"
#include "../camera/CameraFactory.h"


Presenter::Presenter(IView& view, Tracker* tracker, ConfigMgr* conf_mgr)
{
	this->view = &view;
	this->view->connect_presenter(this);

	this->t = tracker;
	this->conf_mgr = conf_mgr;
	ConfigData prefs = conf_mgr->getConfig();

	// Get whether a custom IP was saved or we should set the defaults
	std::string ip_str = prefs.ip;
	int port = prefs.port;
	if (QString(prefs.ip.data()).simplified().replace(" ", "").size() < 2)
	{
		ip_str = network::get_local_ip();
		port = 4242;
	}


	this->udp_sender = new UDPSender(ip_str.data(), prefs.port);

	this->view->set_inputs(prefs);

	CameraFactory camfactory;
	camera = camfactory.buildCamera();
	if (!camera->is_valid)
	{
		std::cout << "[ERROR] NO CAMERAS AVAILABLE" << std::endl;
		this->view->set_enabled(false);
		this->view->show_message("No cameras detected. Plug one and restart the program.", MSG_SEVERITY::CRITICAL);
		
	}
	
}

Presenter::~Presenter()
{
	delete this->udp_sender; 
	delete this->camera;
	delete this->t;
}

void Presenter::sync_ui_inputs()
{
	this->view->set_inputs(conf_mgr->getConfig());
}

void Presenter::run_loop()
{
	FaceData d = FaceData();

	int video_frame_buff_size = camera->width * camera->height * 3;
	uint8_t *video_tex_pixels = new uint8_t[video_frame_buff_size];


	cv::Scalar colorR(255, 0, 0);
	cv::Scalar color(255, 0, 255);

	double buffer_data[6];

	camera->start_camera();

	while(run)
	{
		camera->get_frame(video_tex_pixels);
		cv::Mat mat(camera->height, camera->width, CV_8UC3, video_tex_pixels);

		t->predict(mat, d);

		if (d.face_detected)
		{
			// Paint landmarks
			for (int i = 0; i < 66; i++)
			{
				cv::Point p(d.landmark_coords[2 * i + 1], d.landmark_coords[2 * i]);
				cv::circle(mat, p, 2, color, 3);
			}

			cv::Point p1(d.face_coords[0], d.face_coords[1]);
			cv::Point p2(d.face_coords[2], d.face_coords[3]);
			cv::rectangle(mat, p1, p2, colorR, 1);

			//Send data
			buffer_data[0] = d.translation[0] * 10;
			buffer_data[1] = d.translation[1] * 10;
			buffer_data[2] = d.translation[2] * 10;
			buffer_data[3] = d.rotation[1];   // Yaw
			buffer_data[4] = d.rotation[0];   //Pitch
			buffer_data[5] = d.rotation[2];   //Roll
			udp_sender->send_data(buffer_data);
		}

		cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
		view->paint_video_frame(mat);

		cv::waitKey(35);
	}

	camera->stop_camera();
	delete[] video_tex_pixels;
}

void Presenter::toggle_tracking()
{

	run = !run;
	view->set_tracking_mode(run);
	if (run)
		run_loop();
}

void Presenter::save_prefs(const ConfigData& data)
{
	conf_mgr->updateConfig(data);
}


void Presenter::close_program()
{
	// Assure the camera is released (some cameras have a "recording LED" which can be annoying to have on)
	camera->stop_camera(); 
}