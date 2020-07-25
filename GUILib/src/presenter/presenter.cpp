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




Presenter::Presenter(IView& view, Tracker* tracker) :
	t(640, 480),
	//settings("./prefs.ini", QSettings::IniFormat)
{
	this->view = &view;
	this->view->connect_presenter(this);

	std::string ip_str = network::get_local_ip();
	this->view->set_input_ip(ip_str);  // Set ip input text with the current IP
	this->udp_sender = new UDPSender(ip_str.data(), 5555);
}

Presenter::~Presenter()
{
	delete this->udp_sender;
}

void Presenter::run_loop()
{
	//UDPSender s("192.168.1.137", 5555);
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
	//Tracker t = Tracker(640, 480);


	ctx.eye->start();

	uint8_t* video_tex_pixels = new uint8_t[width * height * 3];


	cv::Scalar colorR(255, 0, 0);
	cv::Scalar color(255, 0, 255);

	double buffer_data[6];

	while(run)
	{
		ctx.eye->getFrame(video_tex_pixels);
		cv::Mat mat(height, width, CV_8UC3, video_tex_pixels);


		t.predict(mat, d);

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
			buffer_data[3] = d.rotation[1];
			buffer_data[4] = d.rotation[0];
			buffer_data[5] = d.rotation[2];
			udp_sender->send_data(buffer_data);
		}
		//cv::imshow("Display window", mat);
		cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
		view->paint_video_frame(mat);

		int key = cv::waitKey(10);
		if (key == 113) break;

	}

	ctx.eye->stop();

	cv::destroyAllWindows();
}

void Presenter::toggle_tracking()
{

	/*QString ip = view->get_input_ip().data();
	QString port = view->get_input_port().data();
	settings.setValue("ip",ip);
	settings.setValue("port", port);*/


	if (view->get_input_ip() != this->udp_sender->ip)
	{
		//std::string msy(this->udp_sender->ip);
		//std::cout << "USER CHANGED IP FIELD: " << view->get_input_ip()  << " UDP sender == " << msy << std::endl;
		delete(this->udp_sender);
		this->udp_sender = new UDPSender(view->get_input_ip().data(), 5555);
	}

	run = !run;
	view->set_tracking_mode(run);
	if (run)
		run_loop();
}