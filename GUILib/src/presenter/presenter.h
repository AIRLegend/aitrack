#pragma once

#include "model.h"


//#include "../view/WindowMain.h"
#include "../model/UDPSender.h"
#include "i_presenter.h"
#include "../view/i_view.h"


class Presenter : IPresenter
{
private:
	FaceData face_data;
	UDPSender udp_sender;
	IView* view;
	bool run = false;

	Tracker t;
public:
	Presenter(IView& view, Tracker *tracker);
	~Presenter();
	void run_loop();
	void toggle_tracking();
};