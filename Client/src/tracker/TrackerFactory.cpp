#include "TrackerFactory.h"

#include <codecvt>
#include <locale>
#include "model.h"

#include "TrackerWrapper.h"

std::unique_ptr<TrackerWrapper> TrackerFactory::buildTracker(
	int im_width, 
	int im_height, 
	float distance, 
	float fov, 
	TRACKER_TYPE type,
	float x_scale,
	float y_scale,
	float z_scale)
{
	std::string landmark_path = model_dir;
	std::string detect_path = model_dir + "detection.onnx";
	bool complex_solver = true;
	bool experimental_model = false;

	std::unique_ptr<ITracker>  t;

	switch(type)
	{
	case TRACKER_TYPE::TRACKER_VERY_FAST:
		landmark_path += "lm_fast_exp1.onnx";
		complex_solver = false;
		experimental_model = true;
		break;
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

	try
	{
		if (experimental_model) {
			// This experimental uses less points and hence, a simple solver
			std::unique_ptr<SimplePositionSolver> solver = std::make_unique<SimplePositionSolver>(
				im_width, im_height, -2, -2, distance, complex_solver, fov, x_scale, y_scale, z_scale
				);
			t = std::make_unique<EfficientTracker>( std::move(solver), detect_wstr, landmark_wstr);
		}
		else {		
			std::unique_ptr<PositionSolver> solver = std::make_unique<PositionSolver>(
				im_width, im_height, -2, -2, distance, complex_solver, fov, x_scale, y_scale, z_scale
			);
			t = std::make_unique<StandardTracker>(std::move(solver), detect_wstr, landmark_wstr);
		}
	}
	catch (std::exception e)
	{
#ifdef _DEBUG
		std::cout << "PROBLEM BUILDING TRACKER:" << e.what() << std::endl;
#endif
		t.reset(nullptr);
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
	names.push_back("Very-Fast");
}

int TrackerFactory::get_type_id(TRACKER_TYPE type)
{
	return (int)type;
}

TRACKER_TYPE TrackerFactory::get_type(int type)
{
	return (TRACKER_TYPE)type;
}
