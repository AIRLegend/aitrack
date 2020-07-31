#include "Config.h"

#include <QFileInfo>



ConfigData ConfigData::getGenericConfig()
{
	ConfigData conf = ConfigData();
	conf.ip = "";
	conf.port = 4242;
	conf.prior_pitch = 0.0;
	conf.prior_yaw = 0.0;
	conf.prior_distance = .6;
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
}

ConfigData ConfigMgr::getConfig()
{
	ConfigData c = ConfigData();
	c.ip = conf.value("ip", "").toString().toStdString();
	c.port = conf.value("port", 4242).toInt();
	c.prior_pitch = conf.value("prior_pitch", 0.0).toDouble();
	c.prior_yaw = conf.value("prior_yaw", 0.0).toDouble();
	c.prior_distance = conf.value("prior_distance", 0.0).toDouble();
	return c;
}

