#pragma once

#include "model.h"

#include "../model/Config.h"
#include "../model/UDPSender.h"
#include "i_presenter.h"
#include "../view/i_view.h"
#include "../camera/ICamera.h"


class Presenter : IPresenter
{
private:
	FaceData face_data;
	UDPSender *udp_sender;
	Tracker *t;
	ICamera *camera;
	
	IView* view;
	bool run = false;
	
	void sync_ui_inputs();
	
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