#include <iostream>

#include "data.h"
#include "model.h"


int main() {
	std::cout << "Model prediction testing" << std::endl;


	const char* imgPath = "C:\\Users\\Álvaro\\Desktop\\customtracker\\test2.jpg";
	cv::Mat image = cv::imread(imgPath);

	FaceData d = FaceData();
	//std::cout << d.rotation[0] << ", " << d.rotation[1] << ", " << d.rotation[2] << std::endl;


	Tracker t = Tracker(640, 480);

	t.predict(image, d);
	//testt();

	// Paint landmarks
	cv::Scalar color(255, 0, 255);
	for (int i = 0; i < 66; i++)
	{
		cv::Point p(d.landmark_coords[2 * i + 1], d.landmark_coords[2 * i]);
		cv::circle(image, p, 2, color, 3);
	}

	cv::imshow("Display window", image);
	cv::waitKey(0);

	return 0;
};



