#include "HeadCalibrationWindow.h"

#include "ConfigWindow.h"
#include <string>

HeadCalibrationWindow::HeadCalibrationWindow(IRootView* prev_window, QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	this->parentView = prev_window;

	btn_calibrate = findChild<QPushButton*>("calibrateBtn");
	view_frame = findChild<QLabel*>("cameraView");

	connect(btn_calibrate, SIGNAL(released()), this, SLOT(onCalibrateClick()));
}

HeadCalibrationWindow::~HeadCalibrationWindow()
{}


void HeadCalibrationWindow::onCalibrateClick()
{
	parentView->notify((IView*)this);

	if (presenter != nullptr)
	{
		this->presenter->calibrate_face(*this);
	}
	else
	{
		throw std::runtime_error("No presenter was linked");
	}
}


void HeadCalibrationWindow::connect_presenter(IPresenter* presenter) 
{ 
	if (presenter != nullptr)
	{
		this->presenter = presenter;
	}
}

void HeadCalibrationWindow::paint_video_frame(cv::Mat& img) 
{ 
	view_frame->setPixmap(
		QPixmap::fromImage(QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888))
		.scaled(400, 280, Qt::KeepAspectRatio, Qt::FastTransformation)
	);
}

void HeadCalibrationWindow::show_tracking_data(ConfigData conf) { }

void HeadCalibrationWindow::set_shortcuts(bool enabled) { }

void HeadCalibrationWindow::set_tracking_mode(bool is_tracking)
{ }

ConfigData HeadCalibrationWindow::get_inputs()
{
	return ConfigData();
}

void HeadCalibrationWindow::update_view_state(ConfigData conf)
{ }

void HeadCalibrationWindow::set_enabled(bool enabled)
{
	this->setEnabled(enabled);
}

void HeadCalibrationWindow::show_message(const char* msg, MSG_SEVERITY severity) {}


void HeadCalibrationWindow::set_visible(bool visible)
{
	this->setHidden(!visible);

	if (!visible)
	{
		// If we hide the view, we should also reset its viewframe
		view_frame->setPixmap(QPixmap());
		view_frame->setText("Look directly at the camera, click \"Calibrate\" and wait a few seconds");
	}
};