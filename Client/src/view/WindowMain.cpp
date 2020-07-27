#include "WindowMain.h"

#include "../presenter/presenter.h"
#include <iostream>

WindowMain::WindowMain(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	this->presenter = NULL;
	btn_track = findChild<QPushButton*>("trackBtn");
	tracking_frame = findChild<QLabel*>("cameraView");

	gp_box_prefs = findChild<QGroupBox*>("prefsGroupbox");
	gp_box_address = gp_box_prefs->findChild<QGroupBox*>("sendGroupbox");
	gp_box_priors = gp_box_prefs->findChild<QGroupBox*>("paramsGroupBox");

	btn_save = gp_box_prefs->findChild<QPushButton*>("saveBtn");

	
	connect(btn_track, SIGNAL(released()), this, SLOT(onTrackClick()));
	connect(btn_save, SIGNAL(released()), this, SLOT(onSaveClick()));

	statusBar()->setSizeGripEnabled(false);
}

WindowMain::~WindowMain()
{
}



void WindowMain::paint_video_frame(cv::Mat& img)
{
	tracking_frame->setPixmap(
		QPixmap::fromImage(QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888))
	);
}

void WindowMain::connect_presenter(IPresenter* presenter)
{
	if (presenter != NULL)
	{
		this->presenter = presenter;
	}
}

void WindowMain::set_tracking_mode(bool is_tracking)
{
	if (is_tracking)
	{
		// Change button name to "stop"
		btn_track->setText("Stop tracking");
		
		// Disable groupbox fields
		gp_box_address->setEnabled(false);
		gp_box_priors->setEnabled(false);
	}
	else
	{
		// Change button name to "start"
		btn_track->setText("Start tracking");

		// Enable groupbox Fields
		gp_box_address->setEnabled(true);
		gp_box_priors->setEnabled(true);

		// Remove background from label
		tracking_frame->setPixmap(QPixmap());
		tracking_frame->setText("No video feed");
	}
}

std::string WindowMain::get_input_ip()
{
	return gp_box_address->findChild<QLineEdit*>("ipField")->text().toStdString();
}

void WindowMain::set_input_ip(std::string& ip)
{
	gp_box_address->findChild<QLineEdit*>("ipField")->setText(ip.data());
}

std::string WindowMain::get_input_port()
{
	return gp_box_address->findChild<QLineEdit*>("portField")->text().toStdString();
}


void WindowMain::onTrackClick()
{
	presenter->toggle_tracking();
}

void WindowMain::onSaveClick()
{
	presenter->save_prefs(get_inputs());
}



ConfigData WindowMain::get_inputs()
{
	ConfigData inputs = ConfigData();
	inputs.ip = gp_box_address->findChild<QLineEdit*>("ipField")->text().toStdString();
	inputs.port= gp_box_address->findChild<QLineEdit*>("portField")->text().toInt();
	inputs.prior_pitch = gp_box_priors->findChild<QLineEdit*>("pitchField")->text().toDouble();
	inputs.prior_yaw = gp_box_priors->findChild<QLineEdit*>("yawField")->text().toDouble();
	inputs.prior_distance = gp_box_priors->findChild<QLineEdit*>("distanceField")->text().toDouble();
	return inputs;
}

void WindowMain::set_inputs(const ConfigData data)
{
	gp_box_address->findChild<QLineEdit*>("ipField")->setText(data.ip.data());
	gp_box_address->findChild<QLineEdit*>("portField")->setText(QString::number(data.port));
	gp_box_priors->findChild<QLineEdit*>("pitchField")->setText(QString::number(data.prior_pitch));
	gp_box_priors->findChild<QLineEdit*>("yawField")->setText(QString::number(data.prior_yaw));
	gp_box_priors->findChild<QLineEdit*>("distanceField")->setText(QString::number(data.prior_distance));

}
