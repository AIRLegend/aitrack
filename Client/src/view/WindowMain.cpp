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
	gp_box = findChild<QGroupBox*>("sendGruoupbox");

	//Connect button signal to appropriate slot
	connect(btn_track, SIGNAL(released()), this, SLOT(onTrackClick()));

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
		
		// Disable IP fields
		gp_box->setEnabled(false);
	}
	else
	{
		// Change button name to "start"
		btn_track->setText("Start tracking");

		// Enable IP Fields
		gp_box->setEnabled(true);

		// Remove background from label
		tracking_frame->setPixmap(QPixmap());
		tracking_frame->setText("No video feed");
	}
}

std::string WindowMain::get_input_ip()
{
	return gp_box->findChild<QLineEdit*>("ipField")->text().toStdString();
}

void WindowMain::set_input_ip(std::string& ip)
{
	gp_box->findChild<QLineEdit*>("ipField")->setText(ip.data());
}

std::string WindowMain::get_input_port()
{
	return gp_box->findChild<QLineEdit*>("portField")->text().toStdString();
}


void WindowMain::onTrackClick()
{
	presenter->toggle_tracking();
}