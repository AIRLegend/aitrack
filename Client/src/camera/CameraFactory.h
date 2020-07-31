#pragma once

#include "ICamera.h"

class CameraFactory
{
public:
	ICamera* buildCamera();
};

