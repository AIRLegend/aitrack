#include "TrackerFactory.h"

#include <codecvt>
#include <locale>
#include "model.h"

#include "TrackerWrapper.h"

std::unique_ptr<ITrackerWrapper> TrackerFactory::buildTracker(int im_width, int im_height, float distance, TRACKER_TYPE type)
{
	std::string landmark_path = model_dir;
	std::string detect_path = model_dir + "detection.onnx";
	bool complex_solver = true;

	switch(type)
	{
	case TRACKER_TYPE::TRACKER_FAST:
		landmark_path += "lm_f.onnx";
		complex_solver = false;
		break;
	case TRACKER_TYPE::TRACKER_MED:
		landmark_path += "lm_m.onnx";
		break;
	case TRACKER_TYPE::TRACKER_FULL:
		landmark_path += "lm_b.onnx";
		break;
	default:
		landmark_path += "lm_m.onnx";
		break;
	}

	std::wstring detect_wstr = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(detect_path);
	std::wstring landmark_wstr = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(landmark_path);

	auto solver = std::make_unique<PositionSolver>(im_width, im_height, -2, -2, distance, complex_solver);

	std::unique_ptr<Tracker> t;
	try
	{
		t = std::make_unique<Tracker>(
			std::move(solver),
			detect_wstr,
			landmark_wstr
		);
	}
	catch (std::exception e)
	{
#ifdef _DEBUG
		std::cout << "PROBLEM BUILDING TRACKER \n" << e.what() << std::endl;
#endif
		t.reset();
	}

	return std::make_unique<TrackerWrapper>(std::move(t), type);
}


TrackerFactory::TrackerFactory(std::string modeldir):
	model_dir(modeldir)
{}

void TrackerFactory::get_model_names(std::vector<std::string>& names)
{
	names.clear();
	names.push_back("Fast");
	names.push_back("Medium");
	names.push_back("Heavy");
}

int TrackerFactory::get_type_id(TRACKER_TYPE type)
{
	return (int)type;
}

TRACKER_TYPE TrackerFactory::get_type(int type)
{
	return (TRACKER_TYPE)type;
}
