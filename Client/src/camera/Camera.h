#pragma once

#include <cstdint>
#include "CameraSettings.h"

class Camera
{
public:
	int width, height, fps;
	bool is_valid = false;

	virtual void start_camera() = 0;
	virtual void stop_camera() = 0;
	virtual void get_frame(uint8_t* buffer) = 0;
	virtual void set_settings(CameraSettings& settings) = 0;
	virtual CameraSettings get_settings() = 0;

	Camera(int width, int height, int fps) {
		this->width = width;
		this->height = height;
		this->fps = fps;
	}
};