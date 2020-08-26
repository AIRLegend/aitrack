#pragma once

#include "ITrackerWrapper.h"
#include "model.h"


// Just to decouple the model library from the application.
class TrackerWrapper : public ITrackerWrapper
{
private:
	TRACKER_TYPE type;
	std::unique_ptr<Tracker> model;

public:
	TrackerWrapper(std::unique_ptr<Tracker>&& model, TRACKER_TYPE type);
	~TrackerWrapper() override;

	// Inherited via ITrackerWrapper
	virtual void predict(cv::Mat& image, FaceData& face_data, const std::unique_ptr<IFilter>& filter = {}) override;
	void update_distance_param(float new_distance) override;
	virtual TRACKER_TYPE get_type() override;
};

