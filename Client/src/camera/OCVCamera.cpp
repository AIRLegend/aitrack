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

	w_scale = (float)width/(float)cam_native_width;
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

		cam_native_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
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
}

void OCVCamera::stop_camera()
{
	cap.release();
}

void OCVCamera::get_frame(uint8_t* buffer)
{
	cv::Mat frame;
	cap.read(frame);
	//Scale maintaining aspect ratio. If distorted, the model will get confused.
	//TODO: Maybe cropping (width,height) section from the center is better.
	cv::resize(frame, frame, size, w_scale, w_scale);
	cv::flip(frame, frame, 1);
	for (int i = 0; i < frame.cols * frame.rows * 3; i++)
		buffer[i] = frame.data[i];

}

void OCVCamera::set_settings(CameraSettings& settings)
{
	this->width = settings.width;
	this->fps = settings.fps;
	this->height = settings.height;
	w_scale = (float)width / (float)cam_native_width;

	// Opencv needs [0,1] ranges
	exposure = settings.exposure < 0 ? -1.0F : (float)settings.exposure/255;
	gain = settings.gain < 0 ? -1.0F : (float)settings.gain / 64;
	cap.set(cv::CAP_PROP_EXPOSURE, exposure);
	cap.set(cv::CAP_PROP_GAIN, gain);
}

CameraSettings OCVCamera::get_settings()
{
	return CameraSettings();
}
