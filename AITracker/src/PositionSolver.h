#pragma once

#include "opencv2/core/matx.hpp"
#include "opencv.hpp"
#include "data.h"

class PositionSolver
{

public:
	PositionSolver(int im_width, int im_height);

	void solve_rotation(FaceData* face_data);

private:
	cv::Mat mat3dface;
	cv::Mat mat3dcontour;
	int contour_indices[18];

	int width, height;
	
	cv::Mat camera_matrix, camera_distortion;
};

