#pragma once

#include "model.h"

#include "../model/Config.h"
#include "../model/UDPSender.h"
#include "../model/UpdateChecker.h"
#include "i_presenter.h"
#include "../view/i_view.h"
#include "../camera/Camera.h"

#include "filters.h"

#include "../tracker/TrackerFactory.h"
#include "../tracker/ITrackerWrapper.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

class Presenter : IPresenter, IUpdateSub
{
private:
	FaceData face_data;
	std::unique_ptr<UDPSender> udp_sender;
	std::unique_ptr<TrackerFactory> tracker_factory;
	std::unique_ptr<ITrackerWrapper> t;
	std::vector<std::shared_ptr<Camera>> all_cameras;
	std::shared_ptr<spdlog::logger> logger;

	// Current program's state + config.
	ConfigData state;

	// Filter which will be aplied to the tracking.
	std::unique_ptr<IFilter> filter;

	IView* view;

	// Whether the main recognition loop has to be running.
	bool run = false;
	// Whether the main recognition loop has to paint the recognized landmarks
	bool paint = true;

	/**
	* Gets the current configuration state and sets the GUI to match it.
	*/
	void sync_ui_inputs();

	/**
	* Stands for initializing or updating the internal UDP sender.
	* If the passed IP and port are equal than the already existing, it
	* does nothing. This method updates the application state.
	*/
	void init_sender(std::string& ip, int port);

	/**
	* Uses the tracker factory to build a new tracker. If the requested one is of the
	* same type as the old one, the call will be ignored.
	* This method updates the application state.
	* @param type Type ID of the desired model (Fast, medium, heavy)
	*/
	void init_tracker(int type);

	/**
	* Uses the internal UDP sender to send the facedata to opentrack.
	*/
	void send_data(double* buffer_data);

	/**
	* runs the recognition loop`.
	*	- Uses the camera to get a frame
	*	- Executes the tracker::predict,
	*	- paints (if needed)  the landmarks on the image
	*	- updates (if needed) the UI with the painted image.
	*/
	void run_loop();

	/**
	* Updates presenter state with the current X,Y,Z,Yaw,Pitch,Roll recognized
	*/
	void update_tracking_data(FaceData& facedata);

	/**
	* Updates the stabilizer applied to the recognized facial landmarks.
	*/
	void update_stabilizer(const ConfigData &data);

	/**
	* Helper function. Returns a CameraSettings config given the current application
	* state
	* @returns camera config given the application state
	*/
	CameraSettings build_camera_params();

	/**
	* Updates the condifuration of the currently selected camera
	*/
	void update_camera_params();

public:
	std::unique_ptr<ConfigMgr> conf_mgr;
	std::unique_ptr<UpdateChecker> update_chkr;

	Presenter(IView& view, std::unique_ptr<TrackerFactory>&& t_factory, std::unique_ptr<ConfigMgr>&& conf_mgr);

	//IPresenter
	void toggle_tracking();
	void save_prefs(const ConfigData& data);
	void close_program();

	//IUpdatesub
	void on_update_check_completed(bool update_exists);
};