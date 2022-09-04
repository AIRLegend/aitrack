#pragma once

#include <QWidget>
#include <QSpinBox>
#include <QMessageBox>
#include <QLineEdit>
#include <QCheckBox>

#include "ui_ConfigWindow.h"
#include "i_view.h"
#include "HeadCalibrationWindow.h"


class ConfigWindow : public QWidget, IView
{
	Q_OBJECT

public:
	ConfigWindow(IRootView *prev_window, QWidget *parent = Q_NULLPTR);
	~ConfigWindow();

	// Inherited via IView
	void connect_presenter(IPresenter* presenter) override;
	void show_tracking_data(ConfigData conf) override;
	void set_tracking_mode(bool is_tracking) override;
	ConfigData get_inputs() override;
	void update_view_state(ConfigData conf) override;
	void set_enabled(bool enabled) override;
	void show_message(const char* msg, MSG_SEVERITY severity) override;
	void set_shortcuts(bool enabled) override;
	IView* get_calibration_window();
	void set_visible(bool visible);
	void paint_video_frame(cv::Mat& img) {};


private:
	Ui::ConfigWindow ui;

	IPresenter* presenter;

	HeadCalibrationWindow calibration_window;

	IRootView *parentView;

	QPushButton *btn_apply, *btn_calibrate;
	QComboBox *input_camera, *cb_modelType;

	QCheckBox *check_stabilization_landmarks, *check_auto_update, *check_enable_tracking_shortcut;
	QLineEdit *distance_param, *fov_param, * ip_field, * port_field;

	QGroupBox *gp_box_camera_prefs, *gp_box_image_prefs, *gp_box_address, *gp_box_priors;;

	QSpinBox *width_selector, *height_selector, *fps_selector;
	QSlider *gain_slider, *exposure_slider;

	

private slots:
	void onApplyClick();
	void onCalibrateClick();
};
