#pragma once

#include "data.h"
#include "filters.h"
#include "opencv2/core/matx.hpp"


/**
* Possible model types. 
* TRACKER_FAST = Less weights. Lighter, but could have less precission
* TRACKER_MED = Half the way. Could offer a good tradeoff.
* TRACKER_FULL = The heaviest. Could perform the best but also eats more CPU. 
*/
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
	/**
	* Detects a face, its landmarks and estimates a translation + rotation position.
	* @param image
	* @param face_data Where the results of the recognition will be stored
	* @param filter Optional filter/operation which will be executed (if != nullptr) on the extracted facial landmarks.
	*/
	virtual void predict(cv::Mat& image, FaceData& face_data, IFilter* filter)=0;
	virtual void update_distance_param(float new_distance) = 0;
	virtual TRACKER_TYPE get_type()=0;
};

