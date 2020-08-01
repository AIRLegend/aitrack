#pragma once

#include "opencv2/core/matx.hpp"
#include "opencv.hpp"
#include "data.h"

class PositionSolver
{

public:

	double prior_pitch, prior_yaw, prior_distance;

	PositionSolver(
		int im_width, 
		int im_height,
		float prior_pitch = -2.f,
		float prior_yaw = -2.f,
		float prior_distance = -1.2f);

	void solve_rotation(FaceData* face_data);

private:
	cv::Mat mat3dface;
	cv::Mat mat3dcontour;
	int contour_indices[18];

	int width, height;
	
	cv::Mat camera_matrix, camera_distortion;

	void get_euler(cv::Mat& rvec, cv::Mat& tvec);
};

