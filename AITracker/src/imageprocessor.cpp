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

    /*float* ptr = image.ptr<float>();
    for (int channel = 0; channel < 3; channel++){
        for (int i = 0; i < 224 * 224; i++) {
            ptr[224*224*channel + i] /= std_scaling[channel];
        }
    }*/

	cv::subtract(image, mean_scaling, image);
}


/*void ImageProcessor::cvt_format(float* from, float* dest, int dim_x, int dim_y)
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

void ImageProcessor::transpose(float* from, float* dest, int dim_x, int dim_y)
{
    int stride = 224 * 224;

    for (int c = 0; c < 3; c++)
    {
        for (int i = 0; i < 224 * 224; i++)
        {
            dest[i + stride*c] = from[c + i*3];
        }
    }
}*/


void ImageProcessor::cvt_format(float* from, float* dest, int dim_x, int dim_y)
{
    for (int channel = 1; channel < 4; channel++)
    {
        for (int row = 0; row < dim_x; row++)
        {
            for (int col = 0; col < dim_y; col++)
            {
                dest[((channel - 1) * dim_x * dim_y) + (dim_y * col + row)] = from[3 * (dim_y * col + row)];
            }
        }
    }
}

void ImageProcessor::transpose(float* from, float* dest, int dim_x, int dim_y)
{
    int stride = dim_x * dim_y;

    for (int c = 0; c < 3; c++)
    {
        for (int i = 0; i < dim_x * dim_y; i++)
        {
            dest[i + stride*c] = from[c + i*3];
        }
    }
}


