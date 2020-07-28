#pragma once
#include "ICamera.h"
#include "opencv.hpp"

class OCVCamera : public ICamera
{
private:
	cv::VideoCapture cap;

	bool is_camera_available();

public:
	OCVCamera(int width = 640, int height = 480, int fps = 30);
	~OCVCamera();
	void start_camera();
	void stop_camera();
	void get_frame(uint8_t* buffer);
};

