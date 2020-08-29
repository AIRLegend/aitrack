#pragma once

#include <memory>
#include <vector>

#include "Camera.h"
#include "CameraSettings.h"

class CameraFactory
{
public:
	std::unique_ptr<Camera> buildCamera(int width, int height, int cam_index = 0, int exposure=-1, int gain=-1);
	std::vector<std::shared_ptr<Camera>> getCameras(CameraSettings& settings);
};

