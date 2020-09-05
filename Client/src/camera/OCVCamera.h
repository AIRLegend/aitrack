#pragma once
#include "Camera.h"
#include "opencv.hpp"

class OCVCamera : public Camera
{
private:
	cv::VideoCapture cap;
	cv::Size size;
	float w_scale;
	float exposure, gain;
	int cam_native_height, cam_native_width, cam_native_fps;
	int cam_index;
	int CV_BACKEND;

	bool is_camera_available();

public:
	OCVCamera(int width = 640, int height = 480, int fps = 30, int index = 0);
	~OCVCamera();
	void start_camera();
	void stop_camera();
	void get_frame(uint8_t* buffer);
	void set_settings(CameraSettings& settings);
	CameraSettings get_settings();
};

