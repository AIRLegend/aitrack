#pragma once

#include "model.h"

#include "../model/Config.h"
#include "../model/UDPSender.h"
#include "i_presenter.h"
#include "../view/i_view.h"
#include "../camera/Camera.h"


class Presenter : IPresenter
{
private:
	FaceData face_data;
	UDPSender *udp_sender = NULL;
	Tracker *t = NULL;
	Camera *camera = NULL;
	
	IView* view;
	bool run = false;
	
	void sync_ui_inputs();

	void init_camera();
	void init_sender(std::string& ip, int port);
	
public:
	ConfigMgr* conf_mgr;

	Presenter(IView& view, Tracker *tracker, ConfigMgr* conf_mgr);
	~Presenter();
	void run_loop();

	//IPresenter
	void toggle_tracking();
	void save_prefs(const ConfigData& data);
	void close_program();
};