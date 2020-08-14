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


	//Iview stuff
	void connect_presenter(IPresenter* presenter);
	void paint_video_frame(cv::Mat& img);
	ConfigData get_inputs();
	void update_view_state(ConfigData conf);
	void set_tracking_mode(bool is_tracking);
	void set_enabled(bool enabled);
	void show_message(const char* msg, MSG_SEVERITY severity);


private:
	Ui::MainWindow ui;
	QPushButton *btn_track, *btn_save;
	QLabel* tracking_frame;
	QGroupBox *gp_box_prefs, *gp_box_address, *gp_box_priors;
	QCheckBox* check_video_preview;
	QComboBox* cb_modelType;

	/**
	* Compacting the window to the content.
	*/
	void readjust_size();

	/**
	* Updates the view with the corresponding program state / config.
	*/
	void set_inputs(ConfigData data);


private slots:
	void onTrackClick();
	void onSaveClick();
};
