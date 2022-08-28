#include <iostream>
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

#ifdef OPTIMIZE_ImageProcessor
    for (int c = 0; c < 3; c++)
    {
        float * from_by_channel = &from[c];
        float * dest_by_channel = &dest[stride * c];
        for (int i = 0; i < stride; i++)
        {
            dest_by_channel[i] = from_by_channel[i*3];
        }
    }
#else
    for (int c = 0; c < 3; c++)
    {
        for (int i = 0; i < dim_x * dim_y; i++)
        {
            dest[i + stride*c] = from[c + i*3];
        }
    }
#endif
}

#ifdef OPTIMIZE_ImageProcessor
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

void ImageProcessor::normalize_and_transpose_and_filter(cv::Mat& image, float* dest, std::vector<float>& scalingFilter, int dim_x, int dim_y)
{
    if (scalingFilter.size() == 0)
        return normalize_and_transpose(image, dest, dim_x, dim_y);

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

            from_reference *= scalingFilter[ i ]; // scale according to initialized filter

            dest[stride * channel + i] = from_reference; /* transpose */
        }
    }
}


float ImageProcessor::getDistance(int x1, int y1, int x2, int y2)
{
    int x_distance = (x1 - x2);
    int y_distance = (y1 - y2);
    float distance = (float)sqrt((x_distance * x_distance) + (y_distance * y_distance));
    return distance;
}


std::vector<float>::size_type ImageProcessor::initFilter_conical(int dim_x, int dim_y)
{
    // create a conical filter
    // 1.0 at center of rectangle dimensions dim_x * dim_y
    // 0.5 at edges of x and y axis through the center
    // provides center weight and linear rolloff towards edges

    int x_center = dim_x / 2;
    int y_center = dim_y / 2;

    float scale_distance;
    if (x_center >= y_center)
        scale_distance = (float)x_center;
    else
        scale_distance = (float)y_center;

    scalingFilter.resize(dim_y * dim_x);
    for (int y = 0; y < dim_y; y++)
    {
        for (int x = 0; x < dim_x; x++)
        {
            float distance = getDistance(x, y, x_center, y_center);
            float scale    = 1.0f - (0.5f * distance / scale_distance);

            scalingFilter[y * dim_x + x] = scale;
            //std::cout << x << "," << y << "," << scale << std::endl;
        }
    }

    return scalingFilter.size();
}


std::vector<float>::size_type ImageProcessor::initFilter_vignette(int dim_x, int dim_y)
{
    // create a vignette filter with a normal(gaussian) distribution
    // 1.0 at center of rectangle dimensions dim_x * dim_y
    // apprx 0.34 at edges of x and y axis through the center
    // provides center weight and gaussian rolloff towards edges

    int x_center = dim_x / 2;
    int y_center = dim_y / 2;

    float scale_distance;
    if (x_center >= y_center)
        scale_distance = (float)x_center;
    else
        scale_distance = (float)y_center;
    float radius = 1.0f;
    float maxImageRadius = radius * getDistance(dim_x, dim_y, x_center, y_center);
  
    scalingFilter.resize(dim_y * dim_x);
    for (int y = 0; y < dim_y; y++)
    {
        for (int x = 0; x < dim_x; x++)
        {
            float distance    = getDistance(x, y, x_center, y_center);
            float radiusRatio = distance / maxImageRadius;
            float scale       = (float)pow( cos(radiusRatio), 4);

            scalingFilter[y * dim_x + x] = scale;
            //std::cout << x << "," << y << "," << scale << std::endl;
        }
    }

    return scalingFilter.size();
}


std::vector<float>::size_type ImageProcessor::initFilter_digitalZoom(int dim_x, int dim_y, int zoom)
{
    // create a digital zoom filter
    // 1.0 within centered rectangle dimensions (dim_x/zoom) * (dim_y/zoom)
    // 0.0 outside of centerd rectangle
    // provides level center weight within zoom factor rectangle and masks surrouding borders

    int x_center = dim_x / 2;
    int y_center = dim_y / 2;

    if (zoom < 1)
        zoom = 1;
    else if (zoom > 4)
        zoom = 4;
    int x_left   = x_center - (x_center / zoom);
    int x_right  = x_center + (x_center / zoom);
    int y_top    = y_center - (y_center / zoom);
    int y_bottom = y_center + (y_center / zoom);

    scalingFilter.resize(dim_y * dim_x);
    for (int y = 0; y < dim_y; y++)
    {
        for (int x = 0; x < dim_x; x++)
        {
            float scale;
            if ((y < y_top)  || (y >= y_bottom) ||
                (x < x_left) || (x >= x_right))
                scale = 0.0f;
            else
                scale = 1.0f;

            scalingFilter[ y * dim_x + x ] = scale;
            //std::cout << x << "," << y << "," << scale << std::endl;
        }
    }

    return scalingFilter.size();
}


#endif