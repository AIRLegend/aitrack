#include "CameraFactory.h"

#include "Ps3Camera.h"
#include "OCVCamera.h"
#include "NullCamera.h"

std::unique_ptr<Camera> CameraFactory::buildCamera(int width, int height, int cam_index, int exposure, int gain)
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
			camera = std::make_unique<OCVCamera>(width, height,60, cam_index);
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


std::vector<std::shared_ptr<Camera>> CameraFactory::getCameras()
{
	std::vector<std::shared_ptr<Camera>> cams;

	cams.clear();

	for (int i = 0; i < 10; i++)
	{
		try
		{
			//c = &OCVCamera(640, 480, 30, i);
			std::shared_ptr<Camera> c = std::make_shared<OCVCamera>(640, 480, 30, i);
			cams.push_back(std::move(c));
			std::cout << "Found ID: " << i << std::endl;
		}
		catch (const std::exception&)
		{
			std::cout << "Not found device" << i << std::endl;
		}
	}

	return cams;
}
