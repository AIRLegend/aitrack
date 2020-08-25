#pragma once

#include "Camera.h"
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


class Ps3Camera : public Camera
{

private:
	ps3eye_context ctx;
	CameraSettings setting;

public:
	Ps3Camera(int width = 640, int height = 480, int fps = 30);
	~Ps3Camera();
	void start_camera();
	void stop_camera();
	void get_frame(uint8_t* buffer);
	void set_settings(CameraSettings& settings);
	CameraSettings get_settings();
};

