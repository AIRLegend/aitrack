#pragma once

#include <QWidget>
#include <QSpinBox>
#include <QMessageBox>
#include <QLineEdit>
#include <QCheckBox>

#include "ui_HeadCalibrationWindow.h"
#include "i_view.h"


class HeadCalibrationWindow : public QMainWindow, public IView
{
	Q_OBJECT

public:
	HeadCalibrationWindow(IRootView* prev_window, QWidget* parent = Q_NULLPTR);
	~HeadCalibrationWindow();

	// Inherited via IView
	void connect_presenter(IPresenter* presenter) override;
	void show_tracking_data(ConfigData conf) override;
	void set_tracking_mode(bool is_tracking) override;
	ConfigData get_inputs() override;
	void update_view_state(ConfigData conf) override;
	void set_enabled(bool enabled) override;
	void show_message(const char* msg, MSG_SEVERITY severity) override;
	void set_shortcuts(bool enabled) override;
	IView* get_calibration_window() { return this; };
	void set_visible(bool visible);

	// Inherited via IPaintable
	void paint_video_frame(cv::Mat& img) override;

private:
	Ui::CalibrateWindow ui;

	IRootView* parentView;
	IPresenter* presenter;

	QPushButton* btn_calibrate;
	QLabel* view_frame;


private slots:
	void onCalibrateClick();
};
