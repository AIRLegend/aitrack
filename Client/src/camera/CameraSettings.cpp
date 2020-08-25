#include "CameraSettings.h"


CameraSettings::CameraSettings()
{
	exposure = -1;
	gain = -1;
}

CameraSettings::CameraSettings(CameraSettings& settings)
{
	exposure = settings.exposure;
	gain = settings.gain;
}

CameraSettings::~CameraSettings()
{}