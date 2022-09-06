#pragma once

#include "ITrackerWrapper.h"
#include "model.h"


/**
* Possible model types.
* TRACKER_VERY_FAST = An experimental (even) faster model which runs about x2 faster than TRACKER_FAST.
* TRACKER_FAST = Less weights. Lighter, but could have less precission
* TRACKER_MED = Half the way. Could offer a good tradeoff.
* TRACKER_FULL = The heaviest. Could perform the best but also eats more CPU.
*/
enum class TRACKER_TYPE
{
	TRACKER_VERY_FAST = 3,
	TRACKER_FAST = 0,
	TRACKER_MED = 1,
	TRACKER_FULL = 2
};



// Just to decouple the model library from the application.
class TrackerWrapper
{
private:
	TRACKER_TYPE type;
	std::unique_ptr<ITracker> model;

public:
	TrackerWrapper(std::unique_ptr<ITracker>&& model, TRACKER_TYPE type);
	~TrackerWrapper();

	virtual void predict(cv::Mat& image, FaceData& face_data, const std::unique_ptr<IFilter>& filter = {});
	void update_distance_param(float new_distance);
	virtual TRACKER_TYPE get_type();
	void calibrate(FaceData& face_data);
	TrackerMetadata get_model_config();
};

