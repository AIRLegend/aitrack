#pragma once

#include "ITrackerWrapper.h"
#include <string.h>
#include <memory>
#include <vector>


/**
* Builds TrackerWrapper objects which will be the ones used for making the detections.
*/
class TrackerFactory
{
private:
	std::string model_dir;
public:
	//std::unique_ptr<ITrackerWrapper> buildTracker(int im_width, int im_height, float distance, float fov, TRACKER_TYPE type);
	TrackerFactory(std::string modeldir);
	std::unique_ptr<ITrackerWrapper> buildTracker(int im_width, 
												  int im_height, 
												  float distance, 
												  float fov, 
												  TRACKER_TYPE type= TRACKER_TYPE::TRACKER_FAST,
												  float x_scale = 1.0f,
												  float y_scale = 2.0f,
												  float z_scale = 1.0f);

	/**
	* Set the list with the string identifiers of the available models.
	*/
	void get_model_names(std::vector<std::string>& names);

	int get_type_id(TRACKER_TYPE type);
	TRACKER_TYPE get_type(int type);
};

