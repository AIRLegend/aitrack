#include "WindowMain.h"

#include "../presenter/presenter.h"
#include "../version.h"
#include <iostream>
#include <QMessageBox>


WindowMain::WindowMain(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	this->layout()->setSizeConstraint(QLayout::SetFixedSize);

	this->setWindowTitle(QString("AITrack %1").arg(AITRACK_VERSION));

	this->conf_win = new ConfigWindow(this);
	this->conf_win->hide();

	this->presenter = NULL;

	btn_track = findChild<QPushButton*>("trackBtn");
	tracking_frame = findChild<QLabel*>("cameraView");


	btn_config = findChild<QPushButton*>("btnConfig");

	check_video_preview = findChild<QCheckBox*>("chkVideoPreview");
	
	connect(btn_track, SIGNAL(released()), this, SLOT(onTrackClick()));
	connect(btn_config, SIGNAL(released()), this, SLOT(onConfigClick()));
	connect(check_video_preview, SIGNAL(released()), this, SLOT(onSaveClick()));

	statusBar()->setSizeGripEnabled(false);
}

WindowMain::~WindowMain()
{}

void WindowMain::closeEvent(QCloseEvent* event)
{
	this->presenter->close_program();
}


void WindowMain::paint_video_frame(cv::Mat& img)
{
	if (check_video_preview->isChecked())
		tracking_frame->setPixmap(
			QPixmap::fromImage(QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888))
				.scaled(400,280, Qt::KeepAspectRatio, Qt::FastTransformation)
		);
}


void WindowMain::show_tracking_data(ConfigData conf)
{
	tracking_info->setText(QString("X: %1, Y: %2, Z: %3")
		.arg((int)conf.x)
		.arg((int)conf.y)
		.arg((int)conf.z));
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
	}
	else
	{
		// Change button name to "start"
		btn_track->setText("Start tracking");

		// Remove background from label
		tracking_frame->setPixmap(QPixmap());
		tracking_frame->setText("No video input");
	}

	btn_config->setDisabled(is_tracking);
	conf_win->set_tracking_mode(is_tracking);
}


void WindowMain::update_view_state(ConfigData conf)
{
	set_inputs(conf);
	//show_tracking_data(conf);

	if (!conf.show_video_feed)
	{
		tracking_frame->hide();
		readjust_size();
	}
	else
	{
		tracking_frame->show();
	}
}

ConfigData WindowMain::get_inputs()
{
	// Obtain inputs of children windows
	ConfigData inputs = conf_win->get_inputs();
	inputs.show_video_feed = check_video_preview->isChecked();
	return inputs;
}

void WindowMain::set_inputs(const ConfigData data)
{
	check_video_preview->setChecked(data.show_video_feed);
	this->conf_win->update_view_state(data);
}

void WindowMain::show_message(const char* msg, MSG_SEVERITY severity)
{
	QMessageBox msgBox;
	switch (severity) {
	case CRITICAL:
		msgBox.setIcon(QMessageBox::Warning);
		break;
	default:
		msgBox.setIcon(QMessageBox::Information);
		break;
	}

	msgBox.setText(msg);
	msgBox.exec();
}

void WindowMain::set_enabled(bool enabled)
{
	btn_track->setEnabled(enabled);

	conf_win->set_enabled(enabled);
}

void WindowMain::onTrackClick()
{
	presenter->toggle_tracking();
}

void WindowMain::onSaveClick()
{
	// Merge with config from this window
	ConfigData config = get_inputs();
	
	presenter->save_prefs(config);

	std::cout << "Saved changes" << std::endl;
}

void WindowMain::onConfigClick()
{
	this->conf_win->show();
}

void WindowMain::readjust_size()
{
	findChild<QWidget*>("centralwidget")->adjustSize();
	adjustSize();
}


void WindowMain::notify(IView* self)
{
	this->onSaveClick();
}