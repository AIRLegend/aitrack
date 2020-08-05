#include "model.h"

#include "onnxruntime_cxx_api.h"
#include "opencv2/core/matx.hpp"

#include <math.h>
#include <omp.h>



Tracker::Tracker(PositionSolver* solver, std::wstring& detection_model_path, std::wstring& landmark_model_path):
    improc()
{
    
    this->solver = solver;

    const wchar_t* modelFile = std::wstring(detection_model_path.begin(), detection_model_path.end()).data();   
	session_options = new Ort::SessionOptions();
    enviro = new Ort::Env(ORT_LOGGING_LEVEL_WARNING, "env");
    
    enviro->DisableTelemetryEvents();

	session_options->SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
	session = new Ort::Session(*enviro, detection_model_path.data(), *session_options);
	allocator = new Ort::AllocatorWithDefaultOptions();
    memory_info = (Ort::MemoryInfo*)allocator->GetInfo();


   
    const wchar_t* modelFile2 = std::wstring(landmark_model_path.begin(), landmark_model_path.end()).data();
    session_lm = new Ort::Session(*enviro, landmark_model_path.data(), *session_options);


    tensor_input_size = tensor_input_dims[1] * tensor_input_dims[2] * tensor_input_dims[3];

    detection_input_node_names = {"input"};
    detection_output_node_names = {"output", "maxpool"};
    landmarks_input_node_names = {"input"};
    landmarks_output_node_names = {"output"};
}

Tracker::~Tracker()
{
    delete this->session;
    delete this->session_lm;
}

void Tracker::predict(cv::Mat& image, FaceData& face_data)
{
    cv::Mat img_copy = image.clone();
    img_copy.convertTo(img_copy, CV_32F);
    cv::cvtColor(img_copy, img_copy, cv::COLOR_BGR2RGB);
    improc.normalize(img_copy);

    detect_face(img_copy, face_data);

    if (face_data.face_detected)
    {
        cv::Point p1(face_data.face_coords[0], face_data.face_coords[1]);
        cv::Point p2(face_data.face_coords[2], face_data.face_coords[3]);
        cv::Mat cropped = img_copy(cv::Rect(p1, p2));

        int height = face_data.face_coords[2] - face_data.face_coords[0];
        int width = face_data.face_coords[3] - face_data.face_coords[1];

        float scale_x = (float)width / 224;
        float scale_y = (float)height / 224;
        detect_landmarks(cropped, face_data.face_coords[0], face_data.face_coords[1], scale_x, scale_y, face_data);

        solver->solve_rotation(&face_data);
    }
}

float logit(float p)
{
    if (p >= 1.0)
        p = 0.99999;
    else if (p <= 0.0)
        p = 0.0000001;

    p = p / (1 - p);
    return log(p) / 16;
}

void Tracker::detect_face(const cv::Mat& image, FaceData& face_data)
{
    cv::Mat resized;
    cv::resize(image, resized, cv::Size(224, 224), NULL, NULL, cv::INTER_LINEAR);
    improc.transpose((float*)resized.data, buffer_data);

    Ort::Value input_tensor = Ort::Value::CreateTensor<float>(*memory_info, buffer_data, tensor_input_size, tensor_input_dims, 4);


    auto output_tensors = session->Run(Ort::RunOptions{ nullptr },
        detection_input_node_names.data(), &input_tensor, 1, detection_output_node_names.data(), 2);

    float* output_arr = output_tensors[0].GetTensorMutableData<float>();
    float* maxpool_arr = output_tensors[1].GetTensorMutableData<float>();


    cv::Mat out(4, tensor_detection_output_dims, CV_32F, output_arr);
    cv::Mat maxpool(4, tensor_detection_output_dims, CV_32F, maxpool_arr);
    

    cv::Mat first(56, 56, CV_32F, out.ptr<float>(0,0));
    cv::Mat second(56, 56, CV_32F, out.ptr<float>(0,1));

    cv::Point p(-1, -1);
    cv::minMaxLoc(first, NULL, NULL, NULL, &p);


    float c = first.at<float>(p);
    float r = second.at<float>(p) * 2 * 56;
    int x = p.x * 4;
    int y = p.y * 4;


    face_data.face_detected = c > .6 ? true : false;
    
    if (face_data.face_detected)
    {
        float face[] = { x - r, y - r, 2 * r, 2 * r };

        //std::cout << face[0] << ", " << face[1] << ", " << face[2] << ", " << face[3] << std::endl;
        float width = image.cols;
        float height = image.rows;

        face[0] *= width / 224;
        face[2] *= width / 224;
        face[1] *= height / 224;
        face[3] *= height / 224;

        proc_face_detect(face, width, height);

        face_data.face_coords[0] = face[0];
        face_data.face_coords[1] = face[1];
        face_data.face_coords[2] = face[2];
        face_data.face_coords[3] = face[3];
    }
                     
}



void Tracker::detect_landmarks(const cv::Mat& image, int x0, int y0, float scale_x, float scale_y, FaceData& face_data)
{
    cv::Mat resized;
    cv::resize(image, resized, cv::Size(224, 224), NULL, NULL, cv::INTER_LINEAR);
    improc.transpose((float*)resized.data, buffer_data);

    Ort::Value input_tensor = Ort::Value::CreateTensor<float>(*memory_info, buffer_data, tensor_input_size, tensor_input_dims, 4);


    auto output_tensors = session_lm->Run(Ort::RunOptions{ nullptr }, 
        landmarks_input_node_names.data(), &input_tensor, 1, landmarks_output_node_names.data(), 1);

    float* output_arr = output_tensors[0].GetTensorMutableData<float>();

    this->proc_heatmaps(output_arr, x0, y0, scale_x, scale_y, face_data);
}


void Tracker::proc_face_detect(float* face, float width, float height)
{
    float x = face[0];
    float y = face[1];
    float w = face[2];
    float h = face[3];

    int crop_x1 = (int)(x - w * 0.09f);
    int crop_y1 = (int)(y - h * 0.450f);
    int crop_x2 = (int)(x + w + w * 0.09f);
    int crop_y2 = (int)(y + h + h * 0.450f);

    face[0] = std::max(0, crop_x1);
    face[1] = std::max(0, crop_y1);
    face[2] = std::min((int)width, crop_x2);
    face[3] = std::min((int)height, crop_y2);
}


void Tracker::proc_heatmaps(float* heatmaps, int x0, int y0, float scale_x, float scale_y, FaceData& face_data)
{
    int heatmap_size = 784; //28 * 28;
    for (int landmark = 0; landmark < 66; landmark++)
    {
        int offset = heatmap_size * landmark;
        int argmax = -100;
        float maxval = -100;
        for (int i = 0; i < heatmap_size; i++)
        {
            if (heatmaps[offset + i] > maxval)
            {
                argmax = i;
                maxval = heatmaps[offset + i];
            }
        }

        int x = argmax / 28;
        int y = argmax % 28;


        float conf = heatmaps[offset + argmax];
        float res = 223;

        int off_x = floor(res * (logit(heatmaps[66 * heatmap_size + offset + argmax])) + 0.1);
        int off_y = floor(res * (logit(heatmaps[2 * 66 * heatmap_size + offset + argmax])) + 0.1);


        float lm_x = (float)y0 + (float)(scale_x * (res * (float(x) / 27.) + off_x));
        float lm_y = (float)x0 + (float)(scale_y * (res * (float(y) / 27.) + off_y));

        face_data.landmark_coords[2 * landmark] = lm_x;
        face_data.landmark_coords[2 * landmark + 1] = lm_y;
    }
}





