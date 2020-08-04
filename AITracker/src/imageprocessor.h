#pragma once

#include "opencv.hpp"

class ImageProcessor
{
public:
	ImageProcessor();
	void normalize(cv::Mat& image);
	void cvt_format(float* from, float* dest, int dim_x = 224, int dim_y=224);
	void transpose(float* from, float* dest, int dim_x = 224, int dim_y=224);

private:
	cv::Scalar mean_scaling, std_scaling;
};

