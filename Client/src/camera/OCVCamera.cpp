#include "OCVCamera.h"


OCVCamera::OCVCamera(int width, int height, int fps) :
	Camera(width, height, fps),
	cap()
{
	if (!is_camera_available())
	{
		throw std::runtime_error("No compatible camera found.");
	}
	cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
	is_valid = true;
}

OCVCamera::~OCVCamera()
{
	stop_camera();
}

bool OCVCamera::is_camera_available()
{
	bool available = false;

	cap.open(0, 0);
	available = cap.isOpened();
	if (available)
		cap.release();

	return available;
}

void OCVCamera::start_camera()
{
	cap.open(0, 0);
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
	cv::flip(frame, frame, 1);
	for (int i = 0; i < frame.cols * frame.rows * 3; i++)
		buffer[i] = frame.data[i];

}
