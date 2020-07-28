#pragma once

#include <cstdint>

class ICamera
{
public:
	virtual void start_camera() = 0;
	virtual void stop_camera() = 0;
	virtual void get_frame(uint8_t* buffer) = 0;
};