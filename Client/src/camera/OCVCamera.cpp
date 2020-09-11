#include "OCVCamera.h"


OCVCamera::OCVCamera(int width, int height, int fps, int index) :
	Camera(width, height, fps),
	size(0, 0),
	cap(),
	cam_index(index)
{
	CV_BACKEND = cv::CAP_DSHOW;
	if (!is_camera_available())
	{
		// Check again with Media foundation backend
		CV_BACKEND = cv::CAP_MSMF;
		if (!is_camera_available())
			throw std::runtime_error("No compatible camera found.");
	}
	is_valid = true;


	if (width < 0 || height < 0)
	{
		this->width = cam_native_width;
		this->height = cam_native_height;
	}
	
	if (fps < 0)
		this->fps = cam_native_fps;

	exposure, gain = -1;
}

OCVCamera::~OCVCamera()
{
	stop_camera();
}

bool OCVCamera::is_camera_available()
{
	bool available = false;

	cap.open(cam_index, CV_BACKEND);
	available = cap.isOpened();
	if (available)
	{
		cv::Mat frame;
		cap.read(frame);
		if (frame.empty())
			return false;

		cam_native_width = (int)cap.get(cv::CAP_PROP_FRAME_WIDTH);
		cam_native_height = (int)cap.get(cv::CAP_PROP_FRAME_HEIGHT);
		cam_native_fps = std::max(30, (int)cap.get(cv::CAP_PROP_FPS));
		cap.release();
	}
	return available;
}

void OCVCamera::start_camera()
{
	cap.open(cam_index, CV_BACKEND);
	if (!cap.isOpened())
	{
		throw std::runtime_error("No compatible camera found.");
	}

	// Force its properties each time we start the camera
	// because if we force them with the device switched off
	// bugs will occur (tiling, for example).
	cap.set(cv::CAP_PROP_FRAME_WIDTH, this->width);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, this->height);
	cap.set(cv::CAP_PROP_FPS, this->fps);
}

void OCVCamera::stop_camera()
{
	cap.release();
}

void OCVCamera::get_frame(uint8_t* buffer)
{
	cv::Mat frame;
	cap.read(frame);
	cv::flip(frame, frame, 1);
	for (int i = 0; i < frame.cols * frame.rows * 3; i++)
		buffer[i] = frame.data[i];

}

void OCVCamera::set_settings(CameraSettings& settings)
{
	this->width = settings.width > 0 ? settings.width : this->cam_native_width;
	this->height = settings.height > 0 ? settings.height : this->cam_native_height;
	this->fps = settings.fps >= 30 ? settings.fps : this->cam_native_fps;

	// Disabled for the moment because of the different ranges in generic cameras.
	//exposure = settings.exposure < 0 ? -1.0F : (float)settings.exposure/255;
	//gain = settings.gain < 0 ? -1.0F : (float)settings.gain / 64;
	//cap.set(cv::CAP_PROP_EXPOSURE, exposure);
	//cap.set(cv::CAP_PROP_GAIN, gain);
}

CameraSettings OCVCamera::get_settings()
{
	return CameraSettings();
}
