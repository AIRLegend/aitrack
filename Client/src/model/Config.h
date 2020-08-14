#pragma once

#include <string>
#include <QSettings>

struct ConfigData
{
	std::string ip;
	int port;
	double prior_pitch, prior_yaw, prior_distance;
	bool show_video_feed;

	std::vector<std::string> model_names; //= {"Fast", "Medium", "Heavy"};
	int selected_model;

	static ConfigData getGenericConfig();
};

class ConfigMgr
{
public:
	ConfigMgr(std::string ini_path);
	void updateConfig(const ConfigData& data);
	ConfigData getConfig();

private:
	QSettings conf;
};