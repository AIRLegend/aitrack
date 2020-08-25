#pragma once

#include "Camera.h"

class CameraFactory
{
public:
	Camera* buildCamera(int width, int height, int exposure=-1, int gain=-1);
};

