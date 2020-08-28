#include "ConfigWindow.h"

ConfigWindow::ConfigWindow(IRootView *prev_window, QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	this->parentView = prev_window;

	// References to UI objects
	gp_box_camera_prefs = findChild<QGroupBox*>("gbCamera");

	btn_apply = findChild<QPushButton*>("applyBtn");

	input_camera = gp_box_camera_prefs->findChild<QComboBox*>("cameraIdSelector");
	width_selector = gp_box_camera_prefs->findChild<QSpinBox*>("imgWidthSelector");
	height_selector = gp_box_camera_prefs->findChild<QSpinBox*>("imgHeightSelector"); 
	fps_selector = gp_box_camera_prefs->findChild<QSpinBox*>("fpsSelector");
	gain_slider = gp_box_camera_prefs->findChild<QSlider*>("sliderGain");
	exposure_slider = gp_box_camera_prefs->findChild<QSlider*>("sliderExposure");

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

void ConfigWindow::set_tracking_mode(bool is_tracking)
{
	set_enabled(!is_tracking);
}

ConfigData ConfigWindow::get_inputs()
{
	ConfigData conf = ConfigData();
	conf.cam_gain = gain_slider->value();
	conf.cam_exposure = exposure_slider->value();
	conf.video_fps = fps_selector->value();
	conf.video_width = width_selector->value();
	conf.video_height = height_selector->value();
	conf.selected_camera = input_camera->currentIndex();
	return conf;
}

void ConfigWindow::update_view_state(ConfigData conf)
{
	std::cout << conf.cam_gain << std::endl;
	gain_slider->setValue(conf.cam_gain);
	exposure_slider->setValue(conf.cam_exposure);
	fps_selector->setValue(conf.video_fps);
	width_selector->setValue(conf.video_width);
	height_selector->setValue(conf.video_height);
	input_camera->clear();
	for (int  i = 0; i < conf.num_cameras_detected; i++)
		input_camera->addItem(QString("Camera %1").arg(i), i);
	input_camera->setCurrentIndex(conf.selected_camera);
}

void ConfigWindow::set_enabled(bool enabled)
{
	gp_box_camera_prefs->setEnabled(enabled);
	btn_apply->setEnabled(enabled);
}

void ConfigWindow::show_message(const char* msg, MSG_SEVERITY severity){}