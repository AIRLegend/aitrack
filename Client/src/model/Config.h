#pragma once

#include <string>
#include <QSettings>


/**
* Struct which holds both the program state and the serializable part of this
* state (aka. preferences).
*/
struct ConfigData
{
	std::string ip;
	int port;
	int video_height;
	int video_width;
	int video_fps;
	double prior_distance, camera_fov;
	bool show_video_feed;
	bool use_landmark_stab;
	bool autocheck_updates;
	bool tracking_shortcut_enabled;

	float x, y, z, yaw, pitch, roll;

	int selected_camera;
	int num_cameras_detected;
	int cam_exposure;
	int cam_gain;

	std::vector<std::string> model_names;
	int selected_model;

	static ConfigData getGenericConfig();
};


/**
* Incharged of building/retrieving/saving a config object.
*/
class ConfigMgr
{
public:
	ConfigMgr(std::string ini_path);
	void updateConfig(const ConfigData& data);
	ConfigData getConfig();

private:
	QSettings conf;
};