#include "Ps3Camera.h"
#include <stdexcept>
#include <iostream>


Ps3Camera::Ps3Camera(int width, int height, int fps):
	Camera(width, height, fps),
	ctx(width, height, fps)
{
	if (!this->ctx.hasDevices())
	{
		std::cout << "SOME ERROR BUILDING" << std::endl;
		throw std::runtime_error("No PS3 Camera found.");
	}
	ctx.eye->setFlip(true);
	this->is_valid = true;
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