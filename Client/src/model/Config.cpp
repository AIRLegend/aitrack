#include "Config.h"

#include <QFileInfo>



ConfigData ConfigData::getGenericConfig()
{
	ConfigData conf = ConfigData();
	conf.ip = "";
	conf.port = 0;
	conf.prior_pitch = 0.0;
	conf.prior_yaw = 0.0;
	conf.prior_distance = .6;
	conf.show_video_feed = true;
	conf.selected_model = 0;
	conf.selected_camera = 0;
	conf.num_cameras_detected = 0;
	conf.video_width = 640;
	conf.video_height = 480;
	conf.video_fps = 30;
	conf.use_landmark_stab = true;
	conf.x, conf.y, conf.z, conf.pitch, conf.yaw, conf.roll = 0;
	conf.cam_exposure = -1;
	conf.cam_gain = -1;
	return conf;
}


ConfigMgr::ConfigMgr(std::string ini_path):
	conf(ini_path.data(), QSettings::IniFormat)
{
	//Check if there was no file
	if(!QFileInfo(ini_path.data()).exists())
	{
		//Thre is no previous configuration so we set a default one
		ConfigData cnf_default = ConfigData::getGenericConfig();
		updateConfig(cnf_default);
	}
}

void ConfigMgr::updateConfig(const ConfigData& data)
{
	conf.setValue("ip", data.ip.data());
	conf.setValue("port", data.port);
	conf.setValue("prior_pitch", data.prior_pitch);
	conf.setValue("prior_yaw", data.prior_yaw);
	conf.setValue("prior_distance", data.prior_distance);
	conf.setValue("video_feed", data.show_video_feed);
	conf.setValue("model", data.selected_model);
	conf.setValue("video_width", data.video_width);
	conf.setValue("video_height", data.video_height);
	conf.setValue("stabilize_landmarks", data.use_landmark_stab);
	conf.setValue("fps", data.video_fps);
	conf.setValue("cam_exposure", data.cam_exposure);
	conf.setValue("cam_gain", data.cam_gain);
	conf.setValue("selected_camera", data.selected_camera);
}

ConfigData ConfigMgr::getConfig()
{
	ConfigData c = ConfigData();
	c.ip = conf.value("ip", "").toString().toStdString();
	c.port = conf.value("port", 0).toInt();
	c.prior_pitch = conf.value("prior_pitch", 0.0).toDouble();
	c.prior_yaw = conf.value("prior_yaw", 0.0).toDouble();
	c.prior_distance = conf.value("prior_distance", 0.0).toDouble();
	c.show_video_feed = conf.value("video_feed", true).toBool();
	c.use_landmark_stab = conf.value("stabilize_landmarks", true).toBool();
	c.selected_model = conf.value("model", 0).toInt();
	c.selected_camera = conf.value("selected_camera", 0).toInt();
	c.video_width = conf.value("video_width", 640).toInt();
	c.video_height = conf.value("video_height", 480).toInt();
	c.video_fps = conf.value("fps", 30).toInt();
	c.cam_exposure= conf.value("cam_exposure", -1).toInt();
	c.cam_gain = conf.value("cam_gain", -1).toInt();
	return c;
}

