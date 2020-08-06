#include "CameraFactory.h"

#include "Ps3Camera.h"
#include "OCVCamera.h"
#include "NullCamera.h"

Camera* CameraFactory::buildCamera()
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
			camera = new OCVCamera;
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

	return camera;
}