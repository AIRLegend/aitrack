#include "CameraSettings.h"


CameraSettings::CameraSettings()
{
	exposure = -1;
	gain = -1;
	width = 640;
	height = 480;
	fps = 30;
}

CameraSettings::CameraSettings(CameraSettings& settings)
{
	exposure = settings.exposure;
	gain = settings.gain;
	fps = settings.fps;
	width = settings.width;
	height = settings.height;
}

CameraSettings::~CameraSettings()
{}