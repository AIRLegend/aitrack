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
	double prior_pitch, prior_yaw, prior_distance;
	bool show_video_feed;
	bool use_landmark_stab;

	float x, y, z, yaw, pitch, roll;

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