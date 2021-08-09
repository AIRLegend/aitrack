#include "ConfigWindow.h"

ConfigWindow::ConfigWindow(IRootView *prev_window, QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	this->parentView = prev_window;

	// References to UI objects
	gp_box_camera_prefs = findChild<QGroupBox*>("gbCamera");
	gp_box_address = findChild<QGroupBox*>("sendGroupbox");
	gp_box_priors = findChild<QGroupBox*>("paramsGroupBox");
	gp_box_image_prefs = gp_box_camera_prefs->findChild<QGroupBox*>("gbImageParams");

	btn_apply = findChild<QPushButton*>("applyBtn");

	input_camera = gp_box_camera_prefs->findChild<QComboBox*>("cameraIdSelector");
	width_selector = gp_box_camera_prefs->findChild<QSpinBox*>("imgWidthSelector");
	height_selector = gp_box_camera_prefs->findChild<QSpinBox*>("imgHeightSelector"); 
	fps_selector = gp_box_camera_prefs->findChild<QSpinBox*>("fpsSelector");
	gain_slider = gp_box_image_prefs->findChild<QSlider*>("sliderGain");
	exposure_slider = gp_box_image_prefs->findChild<QSlider*>("sliderExposure");


	ip_field = gp_box_address->findChild<QLineEdit*>("ipField");
	port_field = gp_box_address->findChild<QLineEdit*>("portField");

	check_auto_update = findChild<QCheckBox*>("updateChckbox");
	check_stabilization_landmarks = gp_box_priors->findChild<QCheckBox*>("landmarkStabChck");
	check_enable_tracking_shortcut = findChild<QCheckBox*>("enableTrackingCheckbox");
	cb_modelType = gp_box_priors->findChild<QComboBox*>("modeltypeSelect");
	distance_param = gp_box_priors->findChild<QLineEdit*>("distanceField");
	fov_param = gp_box_priors->findChild<QLineEdit*>("fovField");


	connect(btn_apply, SIGNAL(released()), this, SLOT(onApplyClick()));
}

ConfigWindow::~ConfigWindow()
{}


void ConfigWindow::onApplyClick() {
	parentView->notify((IView*)this);
}


void ConfigWindow::connect_presenter(IPresenter* presenter) { }

void ConfigWindow::paint_video_frame(cv::Mat& img) { }

void ConfigWindow::show_tracking_data(ConfigData conf) { }

void ConfigWindow::set_shortcuts(bool enabled) { }

void ConfigWindow::set_tracking_mode(bool is_tracking)
{
	set_enabled(!is_tracking);
}

ConfigData ConfigWindow::get_inputs()
{
	ConfigData conf = ConfigData();
	conf.cam_gain = gp_box_image_prefs->isChecked() ? gain_slider->value() : -1;
	conf.cam_exposure = gp_box_image_prefs->isChecked() ? exposure_slider->value() : -1;
	conf.video_fps = fps_selector->value();
	conf.video_width = width_selector->value();
	conf.video_height = height_selector->value();
	conf.selected_camera = input_camera->currentIndex();
	conf.prior_distance = distance_param->text().toDouble();
	conf.camera_fov = fov_param->text().toDouble();
	conf.ip = ip_field->text().toStdString();
	conf.port = port_field->text().toInt();
	conf.use_landmark_stab = check_stabilization_landmarks->isChecked();
	conf.selected_model = cb_modelType->currentIndex();
	conf.autocheck_updates = check_auto_update->isChecked();
	conf.tracking_shortcut_enabled = check_enable_tracking_shortcut->isChecked();

	return conf;
}

void ConfigWindow::update_view_state(ConfigData conf)
{
	std::cout << conf.cam_gain << std::endl;
	if (conf.cam_gain > 0 && conf.cam_exposure > 0)
	{
		gain_slider->setValue(conf.cam_gain);
		exposure_slider->setValue(conf.cam_exposure);
		gp_box_image_prefs->setChecked(true);
	}
	else
	{
		gp_box_image_prefs->setChecked(false);
	}
	fps_selector->setValue(conf.video_fps);
	width_selector->setValue(conf.video_width);
	height_selector->setValue(conf.video_height);
	input_camera->clear();
	for (int  i = 0; i < conf.num_cameras_detected; i++)
		input_camera->addItem(QString("Camera %1").arg(i), i);
	input_camera->setCurrentIndex(conf.selected_camera);

	cb_modelType->clear();
	for (std::string s : conf.model_names)
		cb_modelType->addItem(QString(s.data()));
	cb_modelType->setCurrentIndex(conf.selected_model);

	check_auto_update->setChecked(conf.autocheck_updates);
	check_enable_tracking_shortcut->setChecked(conf.tracking_shortcut_enabled);
	check_stabilization_landmarks->setChecked(conf.use_landmark_stab);
	distance_param->setText(QString::number(conf.prior_distance));
	fov_param->setText(QString::number(conf.camera_fov));


	if (conf.ip != "" || conf.port > 0)
	{
		gp_box_address->setChecked(true);
		ip_field->setText(QString::fromStdString(conf.ip));
		port_field->setText(QString("%1").arg(conf.port));
	}
	else
	{
		gp_box_address->setChecked(false);
		ip_field->setText("");
		port_field->setText("");
	}
	
}

void ConfigWindow::set_enabled(bool enabled)
{
	this->setEnabled(enabled);
}
void ConfigWindow::show_message(const char* msg, MSG_SEVERITY severity){}
