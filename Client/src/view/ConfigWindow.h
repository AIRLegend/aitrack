#pragma once

#include <QWidget>
#include <QSpinBox>
#include <QMessageBox>
#include <QLineEdit>
#include <QCheckBox>

#include "ui_ConfigWindow.h"
#include "i_view.h"


class ConfigWindow : public QWidget, IView
{
	Q_OBJECT

public:
	ConfigWindow(IRootView *prev_window, QWidget *parent = Q_NULLPTR);
	~ConfigWindow();

	// Inherited via IView
	virtual void connect_presenter(IPresenter* presenter) override;
	virtual void paint_video_frame(cv::Mat& img) override;
	virtual void show_tracking_data(ConfigData conf) override;
	virtual void set_tracking_mode(bool is_tracking) override;
	virtual ConfigData get_inputs() override;
	virtual void update_view_state(ConfigData conf) override;
	virtual void set_enabled(bool enabled) override;
	virtual void show_message(const char* msg, MSG_SEVERITY severity) override;
	virtual void set_shortcuts(bool enabled) override;

private:
	Ui::ConfigWindow ui;

	IRootView *parentView;

	QPushButton *btn_apply;
	QComboBox *input_camera, *cb_modelType;

	QCheckBox *check_stabilization_landmarks, *check_auto_update, *check_enable_tracking_shortcut;
	QLineEdit *distance_param, *fov_param, * ip_field, * port_field;

	QGroupBox *gp_box_camera_prefs, *gp_box_image_prefs, *gp_box_address, *gp_box_priors;;

	QSpinBox *width_selector, *height_selector, *fps_selector;
	QSlider *gain_slider, *exposure_slider;

	

private slots:
	void onApplyClick();
};
