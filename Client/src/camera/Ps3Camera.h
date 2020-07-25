#pragma once

#include "ICamera.h"
#include <vector>
#include "ps3eye.h"


struct ps3eye_context {
	ps3eye_context(int width, int height, int fps) :
		eye(0)
		, devices(ps3eye::PS3EYECam::getDevices())
		, running(true)
		, last_ticks(0)
		, last_frames(0)
	{
		if (hasDevices()) {
			eye = devices[0];
			eye->init(width, height, (uint16_t)fps);
		}
	}

	bool hasDevices()
	{
		return (devices.size() > 0);
	}

	std::vector<ps3eye::PS3EYECam::PS3EYERef> devices;
	ps3eye::PS3EYECam::PS3EYERef eye;

	bool running;
	uint32_t last_ticks;
	uint32_t last_frames;
};


class Ps3Camera : public ICamera
{
public:
	Ps3Camera();
	~Ps3Camera();
	void start_camera();
	void stop_camera();
	void get_frame(uint8_t* buffer);
};

