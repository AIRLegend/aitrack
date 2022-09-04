#pragma once

#include "opencv2/opencv.hpp"

#include <onnxruntime_cxx_api.h>
#include "data.h"
#include "imageprocessor.h"
#include "PositionSolver.h"
#include "filters.h"

/**
* Struct for storing and passing around model config and other stuff
*/
struct TrackerMetadata {
	// Face width ratio with respect to the standard 3d face
	float head_width_scale;
};

class ITracker
{
public:
	std::unique_ptr<PositionSolver> solver = 0;

	virtual void predict(cv::Mat& image, FaceData& face_data, const std::unique_ptr<IFilter>& filter = {}) {};
	virtual void calibrate(FaceData& face_data) {};
	virtual TrackerMetadata get_metadata() = 0;

	virtual ~ITracker() {};
};


class StandardTracker : public ITracker
{

public:
	//std::unique_ptr<PositionSolver> solver;

	StandardTracker(std::unique_ptr<PositionSolver>&& solver, std::wstring& detection_model_path, std::wstring& landmark_model_path);
	virtual ~StandardTracker();

	virtual void predict(cv::Mat& image, FaceData& face_data, const std::unique_ptr<IFilter>& filter = {});
	void calibrate(FaceData& face_data);
	TrackerMetadata get_metadata();

protected:
	ImageProcessor improc;
	Ort::AllocatorWithDefaultOptions allocator = {};
	const OrtMemoryInfo* memory_info;

	std::string detection_model_path;
	std::shared_ptr<Ort::Env> enviro;
	std::unique_ptr<Ort::Session> session_lm;

	std::vector<const char*> detection_input_node_names;
	std::vector<const char*> detection_output_node_names;
	std::vector<const char*> landmarks_input_node_names;
	std::vector<const char*> landmarks_output_node_names;

	cv::Ptr<cv::FaceDetectorYN> face_detector;

	size_t tensor_input_size;
	int64_t tensor_input_dims[4] = { 1, 3, 224, 224 };

	// Buffer for transposing the input of the landmark model
	float buffer_data[150528];

	void detect_face(const cv::Mat& image, FaceData& face_data);
	virtual void detect_landmarks(const cv::Mat& image, int x0, int y0, float scale_x, float scale_y, FaceData& face_data);

	// gets the number of items (floats) to be allocated depending on image dimension
	virtual size_t get_lm_input_size();
	virtual const int64_t* get_landmark_input_dims();

private:
	void proc_face_detect(float* face, float width = 1080, float height = 720);
	void proc_heatmaps(float* heatmaps, int x0, int y0, float scale_x, float scale_y, FaceData& face_data);
};


class EfficientTracker: public StandardTracker
{
public:
	EfficientTracker(std::unique_ptr<PositionSolver> solver, std::wstring& detection_model_path, std::wstring& landmark_model_path);

private:
	void detect_landmarks(const cv::Mat& image, int x0, int y0, float scale_x, float scale_y, FaceData& face_data) override;
};