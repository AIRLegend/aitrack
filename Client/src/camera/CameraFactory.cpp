#include "CameraFactory.h"

#include "Ps3Camera.h"
#include "OCVCamera.h"
#include "NullCamera.h"

std::unique_ptr<Camera> CameraFactory::buildCamera(int width, int height, int exposure, int gain)
{
	std::unique_ptr<Camera> camera;
	bool error = false;
	bool has_ps3 = true;

	try
	{
		camera = std::make_unique<Ps3Camera>(640, 480, 30);
	} catch (std::exception)
	{
		has_ps3 = false;
	}

	if (!has_ps3)
	{
		try {
			camera = std::make_unique<OCVCamera>(width, height);
		}
		catch (std::exception)
		{
			error = true;
		}
	}

	if (error)
	{
		camera = std::make_unique<NullCamera>();
	}

	CameraSettings cam_settings;
	cam_settings.exposure = exposure;
	cam_settings.gain = gain;
	camera->set_settings(cam_settings);

	return camera;
}