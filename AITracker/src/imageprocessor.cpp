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
        float * from_by_channel = &from[c];
        float * dest_by_channel = &dest[stride * c];
        for (int i = 0; i < stride; i++)
        {
            dest_by_channel[i] = from_by_channel[i*3];
        }
    }
}


void ImageProcessor::normalize_and_transpose(cv::Mat& image, float* dest, int dim_x, int dim_y)
{
    const int stride = dim_x * dim_y;

    // combine normalize and transpose methods to reduce for loops
    float* from = (float*)image.data;
    for (int channel = 0; channel < 3; channel++)
    {
        float std_scaline_for_channel = (float)std_scaling[channel];
        float mean_scaling_for_channel = (float)mean_scaling[channel];

        for (int i = 0; i < stride; i++)
        {
            float& from_reference = from[channel + i * 3]; // use a reference to reduce indexing
            from_reference /= std_scaline_for_channel; /* remove internal for for loop of cv::divide(image, std_scaling, image); */
            from_reference -= mean_scaling_for_channel; /* remove internal for for loop of cv::subtract(image, mean_scaling, image); */

            dest[stride * channel + i] = from_reference; /* transpose */
        }
    }
}
