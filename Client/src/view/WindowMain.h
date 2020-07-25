#pragma once

#include <QtWidgets/QMainWindow>
#include <QImage>
#include "opencv.hpp"
//#include "../presenter/presenter.h"
#include "../presenter/i_presenter.h"
#include "i_view.h"

#include "ui_MainWindow.h"


class Presenter;

class WindowMain : public QMainWindow, IView
{
	Q_OBJECT

public:
	WindowMain(QWidget *parent = 0);
	~WindowMain();
	IPresenter *presenter;

	//Iview
	void paint_video_frame(cv::Mat& img);
	void connect_presenter(IPresenter* presenter);
	void set_tracking_mode(bool is_tracking) override;
	std::string get_input_ip();
	void set_input_ip(std::string& ip);
	std::string get_input_port();
	
private:
	Ui::MainWindow ui;
	QPushButton* btn_track;
	QLabel* tracking_frame;
	QGroupBox* gp_box;


private slots:
	void onTrackClick();
};
