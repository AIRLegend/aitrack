#pragma once

struct CameraSettings
{
	int exposure;
	int gain;
	int fps;
	int width;
	int height;

	CameraSettings();
	CameraSettings(CameraSettings& settings);
	~CameraSettings();
};

