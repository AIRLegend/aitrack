#pragma once
#include "Camera.h"
#include "opencv.hpp"

class OCVCamera : public Camera
{
private:
	cv::VideoCapture cap;
	cv::Size size;
	int CV_BACKEND;

	bool is_camera_available();

public:
	OCVCamera(int width = 640, int height = 480, int fps = 30);
	~OCVCamera();
	void start_camera();
	void stop_camera();
	void get_frame(uint8_t* buffer);
};

