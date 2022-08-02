#pragma once

#include "opencv.hpp"

#define OPTIMIZE_ImageProcessor 1

class ImageProcessor
{
public:
	ImageProcessor();
	void normalize(cv::Mat& image);
	void cvt_format(float* from, float* dest, int dim_x = 224, int dim_y = 224);
	void transpose(float* from, float* dest, int dim_x = 224, int dim_y = 224);
#ifdef OPTIMIZE_ImageProcessor
	void normalize_and_transpose(cv::Mat& image, float* dest, int dim_x = 224, int dim_y = 224);
#endif

private:
	cv::Scalar mean_scaling, std_scaling;
};

