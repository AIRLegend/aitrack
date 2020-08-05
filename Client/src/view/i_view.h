#pragma once


#include "opencv.hpp"
#include "../model/Config.h"
#include "../presenter/i_presenter.h"

enum MSG_SEVERITY { CRITICAL, NORMAL };

class IView {
public:
	virtual void paint_video_frame(cv::Mat& img) = 0;
	virtual void set_tracking_mode(bool is_tracking) = 0;
	virtual void connect_presenter(IPresenter* presenter) = 0;
	virtual std::string get_input_ip() = 0;
	virtual void set_input_ip(std::string& ip) = 0;
	virtual std::string get_input_port() = 0;

	virtual ConfigData get_inputs() = 0;
	virtual void set_inputs(const ConfigData data) = 0;

	virtual void set_enabled(bool enabled) = 0;

	virtual void show_message(const char* msg, MSG_SEVERITY severity) = 0;
};