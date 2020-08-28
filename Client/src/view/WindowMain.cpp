#include "WindowMain.h"

#include "../presenter/presenter.h"
#include <iostream>
#include <QMessageBox>


WindowMain::WindowMain(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	this->layout()->setSizeConstraint(QLayout::SetFixedSize);

	this->conf_win = new ConfigWindow(this);
	this->conf_win->hide();

	this->presenter = NULL;

	btn_track = findChild<QPushButton*>("trackBtn");
	tracking_frame = findChild<QLabel*>("cameraView");


	gp_box_prefs = findChild<QGroupBox*>("prefsGroupbox");
	gp_box_address = gp_box_prefs->findChild<QGroupBox*>("sendGroupbox");
	gp_box_priors = gp_box_prefs->findChild<QGroupBox*>("paramsGroupBox");

	btn_save = gp_box_prefs->findChild<QPushButton*>("saveBtn");
	btn_config = findChild<QPushButton*>("btnConfig");
	cb_modelType = gp_box_priors->findChild<QComboBox*>("modeltypeSelect");

	check_video_preview = findChild<QCheckBox*>("chkVideoPreview");
	check_stabilization_landmarks = findChild<QCheckBox*>("landmarkStabChck");
	
	connect(btn_track, SIGNAL(released()), this, SLOT(onTrackClick()));
	connect(btn_save, SIGNAL(released()), this, SLOT(onSaveClick()));
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
		
		// Disable groupbox fields
		gp_box_address->setEnabled(false);
		gp_box_priors->setEnabled(false);

		//Disable save button
		btn_save->setEnabled(false);
	}
	else
	{
		// Change button name to "start"
		btn_track->setText("Start tracking");

		btn_save->setEnabled(true);

		// Enable groupbox Fields
		gp_box_address->setEnabled(true);
		gp_box_priors->setEnabled(true);
		//check_video_preview->setCheckable(false);

		// Remove background from label
		tracking_frame->setPixmap(QPixmap());
		tracking_frame->setText("No video input");
	}

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
	ConfigData inputs = ConfigData();
	inputs.ip = gp_box_address->findChild<QLineEdit*>("ipField")->text().toStdString();
	inputs.port= gp_box_address->findChild<QLineEdit*>("portField")->text().toInt();
	inputs.prior_distance = gp_box_priors->findChild<QLineEdit*>("distanceField")->text().toDouble();
	inputs.show_video_feed = check_video_preview->isChecked();
	inputs.selected_model = cb_modelType->currentIndex();
	inputs.use_landmark_stab = check_stabilization_landmarks->isChecked();
	return inputs;
}

void WindowMain::set_inputs(const ConfigData data)
{
	if(data.ip != "" || data.port > 0)
		gp_box_address->setChecked(true);

	gp_box_address->findChild<QLineEdit*>("ipField")->setText(data.ip.data());
	gp_box_address->findChild<QLineEdit*>("portField")->setText(data.port==0 ? "" : QString::number(data.port));
	gp_box_priors->findChild<QLineEdit*>("distanceField")->setText(QString::number(data.prior_distance));
	check_video_preview->setChecked(data.show_video_feed);

	cb_modelType->clear();
	for (std::string s:data.model_names)
		cb_modelType->addItem(QString(s.data()));
	cb_modelType->setCurrentIndex(data.selected_model);
	check_stabilization_landmarks->setChecked(data.use_landmark_stab);

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
	// Obtain inputs of children windows
	ConfigData conf_child = conf_win->get_inputs();

	// Merge with config from this window
	ConfigData config = get_inputs();

	conf_child.ip = config.ip;
	conf_child.port = config.port;
	conf_child.prior_distance = config.prior_distance;
	conf_child.show_video_feed = config.show_video_feed;
	conf_child.selected_model = config.selected_model;
	conf_child.use_landmark_stab = config.use_landmark_stab;
	
	presenter->save_prefs(conf_child);

	std::cout << "Saved changes" << std::endl;
}

void WindowMain::onConfigClick()
{
	this->conf_win->show();
	std::cout << "Config" << std::endl;
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