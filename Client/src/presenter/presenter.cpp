#include <memory>
#include <string.h>

#include "presenter.h"
#include "opencv.hpp"

#include "../camera/CameraFactory.h"


Presenter::Presenter(IView& view, std::unique_ptr<TrackerFactory>&& t_factory, std::unique_ptr<ConfigMgr>&& conf_mgr)
{
	
	logger = spdlog::get("aitrack");

	this->tracker_factory = std::move(t_factory);
	this->conf_mgr = std::move(conf_mgr);
	state = this->conf_mgr->getConfig();

	this->view = &view;
	this->view->connect_presenter(this);
	this->paint = state.show_video_feed;
	this->filter = nullptr;

	// Init available model names to show in the GUI
	this->tracker_factory->get_model_names(state.model_names);

	// Setup a filter to stabilize the recognized facial landmarks if needed.
	update_stabilizer(state);

	CameraFactory camfactory;
	CameraSettings camera_settings = build_camera_params();
	all_cameras = camfactory.getCameras(camera_settings);
	logger->info("Number of recognized cameras: {}", all_cameras.size());

	if (all_cameras.size() == 0)
	{
		std::cout << "[ERROR] NO CAMERAS AVAILABLE" << std::endl;
		this->view->set_enabled(false);
		this->view->show_message("No cameras detected. Plug one and restart the program.", MSG_SEVERITY::CRITICAL);
		logger->info("No cameras were detected");
	}
	else
	{
		//Change the number of available cameras
		state.num_cameras_detected = (int)all_cameras.size();

		// Request sockets (UDP Sender) only if needed.
		std::string ip_str = state.ip;
		int port = state.port;
		init_sender(ip_str, port);

		// Build tracker
		init_tracker(state.selected_model);

	}

	// Check if there was a problem initing tracker
	if (this->t == nullptr)
	{
		this->view->set_enabled(false);
		this->view->show_message("There was a problem initializing the tracker. Check the models folder and restart the program.", MSG_SEVERITY::CRITICAL);
	}
	sync_ui_inputs();
}

void Presenter::init_sender(std::string &ip, int port)
{
	state.ip = ip;
	state.port = port;

	// Updata only if needed.
	if (this->udp_sender)
	{
		if (ip != this->udp_sender->ip && port != this->udp_sender->port)
			return;
	}

	std::string ip_str = ip;
	int port_dest = port;
	if (QString(ip_str.data()).simplified().replace(" ", "").size() < 2)
		ip_str = "127.0.0.1";

	if (port_dest == 0)
		port_dest = 4242;

	this->udp_sender = std::make_unique<UDPSender>(ip_str.data(), port_dest);

	this->logger->info("UDP sender reinitialized. IP: {}  PORT: {}", ip_str, port_dest);
}

void Presenter::init_tracker(int type)
{
	TRACKER_TYPE newtype = tracker_factory->get_type(type);

	if (t != nullptr)
	{
		if (newtype != t->get_type())
		{
#ifdef _DEBUG
			std::cout << "Resetting old tracker" << std::endl;
#endif
			this->logger->info("Rebuilding tracker with new parameters");
			this->t.reset();
			this->t.release();
			this->t = tracker_factory->
				buildTracker(all_cameras[state.selected_camera]->width,
							 all_cameras[state.selected_camera]->height,
							 (float)state.prior_distance,
							 tracker_factory->get_type(type)
				);
		}
		else
		{
			this->t->update_distance_param((float)(this->state.prior_distance));
		}
	}
	else
	{
		this->logger->info("Building Tracker with selected camera: {}", state.selected_camera);
		this->t = tracker_factory->buildTracker(all_cameras[state.selected_camera]->width,
			all_cameras[state.selected_camera]->height,
			(float)state.prior_distance,
			tracker_factory->get_type(type));
	}
	state.selected_model = type;
}


void Presenter::run_loop()
{
	FaceData d = FaceData();

	auto cam = all_cameras[state.selected_camera];

	int video_frame_buff_size = cam->width * cam->height * 3;
	auto video_tex_pixels = std::make_unique<uint8_t[]>(video_frame_buff_size);


	cv::Scalar color_blue(255, 0, 0);
	cv::Scalar color_magenta(255, 0, 255);

	double buffer_data[6];

	this->logger->info("Starting camera {} capture", state.selected_camera);
	cam->start_camera();
	this->logger->info("Camera {} started capturing", state.selected_camera);

	while(run)
	{
		cam->get_frame(video_tex_pixels.get());
		cv::Mat mat(cam->height, cam->width, CV_8UC3, video_tex_pixels.get());

		t->predict(mat, d, this->filter);

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

			update_tracking_data(d);
			send_data(buffer_data);
		}

		if (paint)
		{
			cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
			view->paint_video_frame(mat);
		}

		cv::waitKey(1000/state.video_fps);
	}

	cam->stop_camera();
	this->logger->info("Stop camera {} capture", state.selected_camera);
}


void Presenter::update_tracking_data(FaceData& facedata)
{
	this->state.x = facedata.translation[1] * 10;
	this->state.y = facedata.translation[0] * 10;
	this->state.z = facedata.translation[2] * 10;
	this->state.yaw = facedata.rotation[1];   // Yaw
	this->state.pitch = facedata.rotation[0];   //Pitch
	this->state.roll = facedata.rotation[2];   //Roll
}


void Presenter::update_stabilizer(const ConfigData& data)
{
	// Right now, only enabling/disabling it is supported
	this->state.use_landmark_stab = data.use_landmark_stab;
	if (!state.use_landmark_stab)
	{
		this->filter.reset();
	}
	else
	{
		this->filter = std::make_unique<EAFilter>(66 * 2);
	}
	this->logger->info("Updated stabilizer.");
}

CameraSettings Presenter::build_camera_params()
{
	CameraSettings camera_settings;
	camera_settings.exposure = state.cam_exposure;
	camera_settings.gain = state.cam_gain;
	camera_settings.fps = state.video_fps;
	camera_settings.width = state.video_width;
	camera_settings.height = state.video_height;
	return camera_settings;
}

void Presenter::update_camera_params()
{
	this->logger->info("Update camera parameters.");
	all_cameras[state.selected_camera]->set_settings(build_camera_params());
}


void Presenter::send_data(double* buffer_data)
{
	//Send data
	buffer_data[0] = state.x;
	buffer_data[1] = state.y;
	buffer_data[2] = state.z;
	buffer_data[3] = state.yaw;   // Yaw
	buffer_data[4] = state.pitch;   //Pitch
	buffer_data[5] = state.roll;   //Roll
	udp_sender->send_data(buffer_data);
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
	this->logger->info("Saving prefs");

	// Disable painting parts from the run loop if needed
	this->paint = data.show_video_feed;
	state.show_video_feed = data.show_video_feed;

	this->state.prior_distance = data.prior_distance;


	// Change stabilizer configuration. This will update the internal
	// program state.
	update_stabilizer(data);

	// Reset UDPSender this will also update the state member.
	std::string ip_str = data.ip;
	int port = data.port;
	init_sender(ip_str, port);

	state.selected_camera = data.selected_camera;
	state.cam_exposure = data.cam_exposure;
	state.cam_gain = data.cam_gain;
	state.video_fps = data.video_fps;
	state.video_height = data.video_height;
	state.video_width = data.video_width;
	update_camera_params();


	// Rebuild tracker if needed. This also will take care of updating the
	// state/distance parameter
	init_tracker(data.selected_model);

	conf_mgr->updateConfig(state);
	sync_ui_inputs();
}


void Presenter::sync_ui_inputs()
{
	this->view->update_view_state(state);
}


void Presenter::close_program()
{
	//Assure we stop tracking loop.
	run = false;
	// Assure all cameras are released (some cameras have a "recording LED" which can be annoying to have on)
	for(std::shared_ptr<Camera> cam : all_cameras)
		cam->stop_camera();
}