#pragma once
#include "Camera.h"

class NullCamera : public Camera
{
public:
	NullCamera(int width = -1, int height = -1, int fps = 0) : Camera(width, height, fps){};
	~NullCamera() {};
	void start_camera() {};
	void stop_camera() {};
	void get_frame(uint8_t* buffer) {};
	void set_settings(CameraSettings& settings) {};
	CameraSettings get_settings() { return CameraSettings(); };
};
