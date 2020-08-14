#pragma once

#include "ITrackerWrapper.h"
#include "model.h"

class TrackerWrapper:ITrackerWrapper
{
private:
	TRACKER_TYPE type;
	Tracker* model;

public:
	TrackerWrapper(Tracker* model, TRACKER_TYPE type);
	~TrackerWrapper() override;

	// Inherited via ITrackerWrapper
	virtual void predict(cv::Mat& image, FaceData& face_data, IFilter* filter) override;
	virtual TRACKER_TYPE get_type() override;
};

