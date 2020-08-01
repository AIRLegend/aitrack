#pragma once

#include <cstdint>

class Camera
{
public:
	int width, height, fps;

	virtual void start_camera() = 0;
	virtual void stop_camera() = 0;
	virtual void get_frame(uint8_t* buffer) = 0;

	Camera(int width, int height, int fps) {
		this->width = width;
		this->height = height;
		this->fps = fps;
	}
};