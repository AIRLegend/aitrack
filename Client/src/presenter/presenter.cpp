//#include "../model/UDPSender.h"

#include "presenter.h"

#include "ps3eye.h"
#include <string.h>

#include "opencv.hpp"

#include "../model/IPResolver.h"



struct ps3eye_context {
	ps3eye_context(int width, int height, int fps) :
		eye(0)
		, devices(ps3eye::PS3EYECam::getDevices())
		, running(true)
		, last_ticks(0)
		, last_frames(0)
	{
		if (hasDevices()) {
			eye = devices[0];
			eye->init(width, height, (uint16_t)fps);
		}
	}

	bool hasDevices()
	{
		return (devices.size() > 0);
	}

	std::vector<ps3eye::PS3EYECam::PS3EYERef> devices;
	ps3eye::PS3EYECam::PS3EYERef eye;

	bool running;
	uint32_t last_ticks;
	uint32_t last_frames;
};



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

	//std::cout << ip_str << "   PORT  " << port << std::endl;

	//this->view->set_input_ip(prefs.ip);
	this->udp_sender = new UDPSender(ip_str.data(), prefs.port);

	this->view->set_inputs(prefs);
}

Presenter::~Presenter()
{
	delete this->udp_sender;
}

void Presenter::sync_ui_inputs()
{
	this->view->set_inputs(conf_mgr->getConfig());
}

void Presenter::run_loop()
{
	int width = 640;
	int height = 480;
	int fps = 30;

	ps3eye_context ctx(width, height, fps);
	if (!ctx.hasDevices()) {
		printf("No PS3 Eye camera connected\n");
		return;
	}
	ctx.eye->setFlip(true);

	FaceData d = FaceData();


	ctx.eye->start();

	int video_frame_buff_size = width * height * 3;
	uint8_t* video_tex_pixels = new uint8_t[video_frame_buff_size];


	cv::Scalar colorR(255, 0, 0);
	cv::Scalar color(255, 0, 255);

	double buffer_data[6];

	while(run)
	{
		ctx.eye->getFrame(video_tex_pixels);
		cv::Mat mat(height, width, CV_8UC3, video_tex_pixels);

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

		cv::waitKey(10);

	}

	ctx.eye->stop();

	//cv::destroyAllWindows();
}

void Presenter::toggle_tracking()
{


	/*if (view->get_input_ip() != this->udp_sender->ip)
	{
		//std::string msy(this->udp_sender->ip);
		//std::cout << "USER CHANGED IP FIELD: " << view->get_input_ip()  << " UDP sender == " << msy << std::endl;
		delete(this->udp_sender);
		this->udp_sender = new UDPSender(view->get_input_ip().data(), 5555);
	}*/

	run = !run;
	view->set_tracking_mode(run);
	if (run)
		run_loop();
}

void Presenter::save_prefs(const ConfigData& data)
{
	conf_mgr->updateConfig(data);
}
