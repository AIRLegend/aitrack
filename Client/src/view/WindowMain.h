#pragma once

#include "i_view.h"

#include <QtWidgets/QMainWindow>
#include <QImage>


#include "opencv.hpp"
#include "../presenter/i_presenter.h"

#include "ui_MainWindow.h"


class Presenter;

class WindowMain : public QMainWindow, IView
{
	Q_OBJECT

public:
	WindowMain(QWidget *parent = 0);
	~WindowMain();
	IPresenter *presenter;
	void closeEvent(QCloseEvent* event) override;

	//Iview
	void paint_video_frame(cv::Mat& img);
	void connect_presenter(IPresenter* presenter);
	void set_tracking_mode(bool is_tracking);
	std::string get_input_ip();
	void set_input_ip(std::string& ip);
	std::string get_input_port();
	ConfigData get_inputs();
	void set_inputs(ConfigData data);
	void show_message(const char* msg, MSG_SEVERITY severity);
	void set_enabled(bool enabled);


	
private:
	Ui::MainWindow ui;
	QPushButton *btn_track, *btn_save;
	QLabel* tracking_frame;
	QGroupBox *gp_box_prefs, *gp_box_address, *gp_box_priors;


private slots:
	void onTrackClick();
	void onSaveClick();
};
