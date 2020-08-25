#include "CameraFactory.h"

#include "Ps3Camera.h"
#include "OCVCamera.h"
#include "NullCamera.h"

Camera* CameraFactory::buildCamera(int width, int height, int exposure, int gain)
{
	Camera *camera = NULL;
	bool error = false;
	bool has_ps3 = true;

	try
	{
		camera = new Ps3Camera(640, 480, 30);
	} catch (std::exception)
	{
		has_ps3 = false;
	}

	if (!has_ps3)
	{
		try {
			camera = new OCVCamera(width, height);
		}
		catch (std::exception)
		{
			error = true;
		}
	}

	if (error)
	{
		delete camera;
		camera = new NullCamera;
	}

	CameraSettings cam_settings;
	cam_settings.exposure = exposure;
	cam_settings.gain = gain;
	camera->set_settings(cam_settings);

	return camera;
}