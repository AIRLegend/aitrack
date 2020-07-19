#pragma once

#define DETECTION_MODEL_N_INS 1
#define DETECTION_MODEL_N_OUTS 2

#define LANDMARK_MODEL_N_INS 1
#define LANDMARK_MODEL_N_OUTS 2


#include "opencv.hpp"
#include <onnxruntime_cxx_api.h>
#include "data.h"
#include "imageprocessor.h"
#include "PositionSolver.h"


class Tracker
{

public:
	Tracker(int img_width, int img_heigth);
	void predict(cv::Mat& image, FaceData& face_data);

private:
	FaceData* facedata; 
	PositionSolver *solver;
	ImageProcessor improc;

	std::string detection_model_path;
	Ort::Env* enviro;
	Ort::SessionOptions* session_options;
	Ort::Session* session;
	Ort::Session* session_lm;
	Ort::AllocatorWithDefaultOptions* allocator;
	Ort::MemoryInfo* memory_info;

	//std::string detection_in_name, detection_out1_name, detection_out2_name;

	std::vector<const char*> detection_input_node_names;
	std::vector<const char*> detection_output_node_names;
	std::vector<const char*> landmarks_input_node_names;
	std::vector<const char*> landmarks_output_node_names;


	size_t tensor_input_size;
	int64_t tensor_input_dims[4] = { 1,3,224,224 };
	int tensor_detection_output_dims[4] = { 1,2,56,56 };

	float buffer_data[150528];
	//Ort::Value buffer_input_tensor;


	void detect_face(const cv::Mat& image, FaceData& face_data);
	void proc_face_detect(float* face, float width = 1080, float height = 720);
	void detect_landmarks(const cv::Mat& image, int x0, int y0, float scale_x, float scale_y, FaceData& face_data);
	void proc_heatmaps(float* heatmaps, int x0, int y0, float scale_x, float scale_y, FaceData& face_data);
};

void load_img(const char* img_path);
