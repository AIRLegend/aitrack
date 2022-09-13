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
		camera = std::make_unique<Ps3Camera>(640, 480, 60);
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

	return camera;
}


std::vector<std::shared_ptr<Camera>> CameraFactory::getCameras(CameraSettings& settings)
{
	std::vector<std::shared_ptr<Camera>> cams;

	for (int i = 0; i < 5; i++)
	{
		bool bFoundPs3Camera = false;
		// Search first for any PS3 camera.
		try
		{
			std::shared_ptr<Camera> c = std::make_shared<Ps3Camera>(640, 480, 60);
			c->set_settings(settings);  // Brightness / Exposure
			cams.push_back(std::move(c));
			bFoundPs3Camera = true;
			std::cout << "Found PS3 camera ID: " << i << std::endl;
		}
		catch (...)
		{
		}

		if (!bFoundPs3Camera)
		{
			// Then search or OCV Camera.
			try
			{
				std::shared_ptr<Camera> c = std::make_shared<OCVCamera>(settings.width, settings.height, settings.fps, i);
				c->set_settings(settings);  // Brightness / Exposure
				cams.push_back(std::move(c));
				std::cout << "Found OCV camera ID: " << i << std::endl;
			}
			catch (const std::exception&)
			{
			}
		}

	}
	if (cams.size() == 0)
	{
		std::cout << "No cameras found" << std::endl;
	}

	return cams;
}
