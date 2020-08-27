#pragma once

#include <memory>

#include "Camera.h"

class CameraFactory
{
public:
	std::unique_ptr<Camera> buildCamera(int width, int height, int exposure=-1, int gain=-1);
};

