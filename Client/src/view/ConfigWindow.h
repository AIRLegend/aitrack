#pragma once

#include "ui_ConfigWindow.h"

#include <QWidget>
#include <QSpinBox>
/*#include <QtWidgets\qpushbutton.h>
#include <QtWidgets\qcombobox.h>
#include <QtWidgets\qgroupbox.h>*/


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

private:
	Ui::ConfigWindow ui;

	IRootView *parentView;

	QPushButton *btn_apply;
	QComboBox *input_camera;

	QGroupBox *gp_box_camera_prefs;

	QSpinBox *width_selector, *height_selector, *fps_selector;
	QSlider *gain_slider, *exposure_slider;

private slots:
	void onApplyClick();
};
