#include "Ps3Camera.h"
#include <stdexcept>
#include <iostream>


Ps3Camera::Ps3Camera(int width, int height, int fps) :
	Camera(width, height, fps),
	ctx(width, height, fps),
	setting()
{
	if (!this->ctx.hasDevices() || ctx.eye == nullptr)
	{
		throw std::runtime_error("No PS3 Camera found.");
	}
	ctx.eye->setFlip(true);
	this->is_valid = true;

	setting.exposure = 140;
	setting.gain = 1;
}

Ps3Camera::~Ps3Camera()
{
	this->stop_camera();
}


void Ps3Camera::start_camera() 
{
	this->ctx.eye->start();
}

void Ps3Camera::stop_camera() 
{
	this->ctx.eye->stop();
}

void Ps3Camera::get_frame(uint8_t *buffer) 
{
	this->ctx.eye->getFrame(buffer);
}

void Ps3Camera::set_settings(CameraSettings& settings)
{
	if (settings.exposure >= 0)
	{
		setting.exposure = min(settings.exposure, 254);
		ctx.eye->setExposure(setting.exposure);
	}

	if (settings.gain >= 0)
	{
		setting.gain = min(settings.gain, 60);
		ctx.eye->setGain(setting.gain);
	}
	this->fps = settings.fps;
	this->ctx.eye->setFrameRate((uint8_t)fps);
}

CameraSettings Ps3Camera::get_settings()
{
	return CameraSettings(setting);
}
