#pragma once

#include "ITrackerWrapper.h"
#include <string.h>
#include <vector>


/**
* Builds TrackerWrapper objects which will be the ones used for making the detections.
*/
class TrackerFactory
{
private:
	std::string model_dir;
public:
	TrackerFactory(std::string modeldir);
	ITrackerWrapper* buildTracker(int im_width, int im_height, float distance, TRACKER_TYPE type= TRACKER_TYPE::TRACKER_FAST);

	/**
	* Set the list with the string identifiers of the available models.
	*/
	void get_model_names(std::vector<std::string>& names);

	int get_type_id(TRACKER_TYPE type);
	TRACKER_TYPE get_type(int type);
};

