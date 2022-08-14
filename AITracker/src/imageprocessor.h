#pragma once

#include "opencv2/opencv.hpp"

class ImageProcessor
{
public:
	ImageProcessor();
	void normalize(cv::Mat& image);
	void cvt_format(float* from, float* dest, int dim_x = 224, int dim_y = 224);
	void transpose(float* from, float* dest, int dim_x = 224, int dim_y = 224);

	/*
	 Alternative implementation of the normalize and transpose functions which executes the same 
	 functionality in a single run. Using this processing time cuts in half.
	*/
	void normalize_and_transpose(cv::Mat& image, float* dest, int dim_x = 224, int dim_y = 224);

private:
	cv::Scalar mean_scaling, std_scaling;
};

