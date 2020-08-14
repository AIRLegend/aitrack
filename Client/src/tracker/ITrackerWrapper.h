#pragma once

#include "data.h"
#include "filters.h"
#include "opencv2/core/matx.hpp"


enum class TRACKER_TYPE
{
	TRACKER_FAST,
	TRACKER_MED,
	TRACKER_FULL
};

class ITrackerWrapper
{
public:
	virtual ~ITrackerWrapper() {};
	virtual void predict(cv::Mat& image, FaceData& face_data, IFilter* filter)=0;
	virtual TRACKER_TYPE get_type()=0;
};

