#include "TrackerWrapper.h"


TrackerWrapper::TrackerWrapper(std::unique_ptr<Tracker>&& model, TRACKER_TYPE type)
{
    this->model = std::move(model);
    this->type = type;
}

TrackerWrapper::~TrackerWrapper()
{
}

void TrackerWrapper::predict(cv::Mat& image, FaceData& face_data, const std::unique_ptr<IFilter>& filter)
{
    model->predict(image, face_data, filter);
}

void TrackerWrapper::update_distance_param(float new_distance)
{
    this->model->solver->set_prior_distance(new_distance);
}

TRACKER_TYPE TrackerWrapper::get_type()
{
    return type;
}
