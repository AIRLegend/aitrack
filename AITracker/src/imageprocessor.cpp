#include "imageprocessor.h"


ImageProcessor::ImageProcessor() : 
	mean_scaling(0.485f, 0.456f, 0.406f),
	std_scaling(0.229f, 0.224f, 0.225f)
{
	//mean_scaling = mean_scaling / std_scaling;
	cv::divide(mean_scaling, std_scaling, mean_scaling);
	std_scaling *= 255.0f;
}

void ImageProcessor::normalize(cv::Mat& image) 
{
	cv::divide(image, std_scaling, image);
	cv::subtract(image, mean_scaling, image);
}


void ImageProcessor::cvt_format(float* from, float* dest, int dim)
{
    for (int channel = 1; channel < 4; channel++)
    {
        for (int row = 0; row < 224; row++)
        {
            for (int col = 0; col < 224; col++)
            {
                dest[((channel - 1) * 224 * 224) + (224 * col + row)] = from[3 * (224 * col + row)];
            }
        }
    }
}

void ImageProcessor::transpose(float* from, float* dest, int dim)
{
    int stride = 224 * 224;

    for (int c = 0; c < 3; c++)
    {
        for (int i = 0; i < 224 * 224; i++)
        {
            dest[i + stride*c] = from[c + i*3];
        }
    }
}


