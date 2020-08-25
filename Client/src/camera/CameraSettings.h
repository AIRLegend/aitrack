#pragma once

struct CameraSettings
{
	int exposure;
	int gain;

	CameraSettings();
	CameraSettings(CameraSettings& settings);
	~CameraSettings();
};

