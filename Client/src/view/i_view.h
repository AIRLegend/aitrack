#pragma once


#include "opencv.hpp"
#include "../model/Config.h"
#include "../presenter/i_presenter.h"

enum MSG_SEVERITY { CRITICAL, NORMAL };



class IView {
public:
	/**
	* Registers a presenter as an observer of this view
	*/
	virtual void connect_presenter(IPresenter* presenter) = 0;

	/**
	* Paint a video frame on the video preview
	*/
	virtual void paint_video_frame(cv::Mat& img) = 0;

	/**
	* Show the tracking data on the UI
	*/
	virtual void show_tracking_data(ConfigData conf) = 0;


	/**
	* Sets the view in tracking or not tracking mode 
	* (make changes on the GUI or update states)
	*/
	virtual void set_tracking_mode(bool is_tracking) = 0;

	/**
	* The view returns a configuration object with the state of all its inputs
	*/
	virtual ConfigData get_inputs() = 0;

	/**
	* The view sets all its inputs to the desired configuration state.
	*/
	virtual void update_view_state(ConfigData conf) = 0;


	/**
	* Disables the GUI in case of error (or something like that) so the user
	* cant start using the program.
	*/
	virtual void set_enabled(bool enabled) = 0;

	/**
	* For showing dialogs or alerts to the user.
	*/
	virtual void show_message(const char* msg, MSG_SEVERITY severity) = 0;
};