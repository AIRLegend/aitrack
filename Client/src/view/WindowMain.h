#pragma once

#include "i_view.h"

#include <QtWidgets/QMainWindow>
#include <QImage>


#include "opencv2/opencv.hpp"
#include "../presenter/i_presenter.h"

#include "ui_MainWindow.h"

#include "ConfigWindow.h"

#include "../utils/qglobalshortcuts/qglobalshortcut.h"

//class IPresenter;

class WindowMain : public QMainWindow,  public IRootView, public IView
{
	Q_OBJECT

public:
	WindowMain(QWidget *parent = 0);
	~WindowMain();
	IPresenter *presenter;
	void closeEvent(QCloseEvent* event) override;

	//Iview stuff
	void connect_presenter(IPresenter* presenter);
	void show_tracking_data(ConfigData conf);
	ConfigData get_inputs();
	void update_view_state(ConfigData conf);
	void set_tracking_mode(bool is_tracking);
	void set_enabled(bool enabled);
	void show_message(const char* msg, MSG_SEVERITY severity);
	void set_shortcuts(bool enabled);
	IView* get_calibration_window();
	void set_visible(bool visible) {};

	//IRootView
	void notify(IView *self);

	//IPaintable
	void paint_video_frame(cv::Mat& img);

private:
	Ui::MainWindow ui;

	//Shortcuts
	QGlobalShortcut *toggle_tracking_shortcut{nullptr};

	QPushButton *btn_track, *btn_save, *btn_config;
	QLabel *tracking_frame, *tracking_info;
	//QGroupBox *gp_box_prefs, *gp_box_address, *gp_box_priors;
	QCheckBox *check_video_preview, *check_stabilization_landmarks;
	//QComboBox* cb_modelType;

	ConfigWindow *conf_win;

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
	void onConfigClick();
};
