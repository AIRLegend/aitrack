

#include "UDPSender.h"

#include <iostream>
#include "ps3eye.h"
#include <string.h>

#include "opencv.hpp"


#include "../AITracker/src/model.h"



struct ps3eye_context {
	ps3eye_context(int width, int height, int fps) :
		eye(0)
		, devices(ps3eye::PS3EYECam::getDevices())
		, running(true)
		, last_ticks(0)
		, last_frames(0)
	{
		if (hasDevices()) {
			eye = devices[0];
			eye->init(width, height, (uint16_t)fps);
		}
	}

	bool hasDevices()
	{
		return (devices.size() > 0);
	}

	std::vector<ps3eye::PS3EYECam::PS3EYERef> devices;
	ps3eye::PS3EYECam::PS3EYERef eye;

	bool running;
	uint32_t last_ticks;
	uint32_t last_frames;
};



void run_camera(int width, int height, int fps)
{

	UDPSender s("192.168.1.137", 5555);

	ps3eye_context ctx(width, height, fps);
	if (!ctx.hasDevices()) {
		printf("No PS3 Eye camera connected\n");
		return;
	}
	ctx.eye->setFlip(true); /* mirrored left-right */


	FaceData d = FaceData();
	Tracker t = Tracker(640, 480);


	ctx.eye->start();

	uint8_t* video_tex_pixels = new uint8_t[width * height * 3];


	cv::Scalar colorR(255, 0, 0);
	cv::Scalar color(255, 0, 255);

	double buffer_data[6];

	while (true)
	{
		ctx.eye->getFrame(video_tex_pixels);
		cv::Mat mat(height, width, CV_8UC3, video_tex_pixels);


		t.predict(mat, d);

		if (d.face_detected)
		{
			// Paint landmarks
			for (int i = 0; i < 66; i++)
			{
				cv::Point p(d.landmark_coords[2 * i + 1], d.landmark_coords[2 * i]);
				cv::circle(mat, p, 2, color, 3);
			}

			cv::Point p1(d.face_coords[0], d.face_coords[1]);
			cv::Point p2(d.face_coords[2], d.face_coords[3]);
			cv::rectangle(mat, p1, p2, colorR, 1);

			//Send data
			buffer_data[0] = d.translation[0] * 10;
			buffer_data[1] = d.translation[1] * 10;
			buffer_data[2] = d.translation[2] * 10;
			buffer_data[3] = d.rotation[1];
			buffer_data[4] = d.rotation[0];
			buffer_data[5] = d.rotation[2];
			s.send_data(buffer_data);
		}
		cv::imshow("Display window", mat);

		int key = cv::waitKey(10);
		if (key == 113) break;
		
	}

	ctx.eye->stop();

	cv::destroyAllWindows();
}

int main(int argc, char* argv[])
{
	bool mode_test = false;
	int width = 640;
	int height = 480;
	int fps = 30;


	

	run_camera(width, height, fps);


	return EXIT_SUCCESS;
}