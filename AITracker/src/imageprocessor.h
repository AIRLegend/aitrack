#pragma once

#include <vector>
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
	void normalize_and_transpose_and_filter(cv::Mat& image, float* dest, std::vector<float>& scalingFilter, int dim_x = 224, int dim_y = 224);
	float getDistance(int x, int y, int x_center, int y_center);
	std::vector<float>::size_type initFilter_vignette(int dim_x = 224, int dim_y = 224);
	std::vector<float>::size_type initFilter_conical(int dim_x = 224, int dim_y = 224);
	std::vector<float>::size_type initFilter_digitalZoom(int dim_x = 224, int dim_y = 224, int zoom = 2);
	std::vector<float> scalingFilter;
#endif

private:
	cv::Scalar mean_scaling, std_scaling;
};

