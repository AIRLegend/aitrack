#include "model.h"

#include "onnxruntime_cxx_api.h"
#include "opencv2/core/matx.hpp"

#include <math.h>
#include <omp.h>


Tracker::Tracker(std::unique_ptr<PositionSolver>&& solver, std::wstring& detection_model_path, std::wstring& landmark_model_path):
    improc(),
    memory_info(allocator.GetInfo()),
    enviro(std::make_unique<Ort::Env>(ORT_LOGGING_LEVEL_WARNING, "env")),
    detection_input_node_names{ "input" },
    detection_output_node_names{ "output", "maxpool" },
    landmarks_input_node_names{ "input" },
    landmarks_output_node_names{ "output" }
{
    this->solver = std::move(solver);

    auto session_options = Ort::SessionOptions();
    session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
    session_options.SetInterOpNumThreads(1);
    session_options.SetIntraOpNumThreads(1);
    session_options.SetExecutionMode(ExecutionMode::ORT_PARALLEL);

    enviro->DisableTelemetryEvents();

   // Landmark detector
    session_lm = std::make_unique<Ort::Session>(*enviro, landmark_model_path.data(), session_options);

    // Face detector
    float score_threshold = .8f;
    float nms_threshold = .3f;
    int topK = 50;
    face_detector = cv::FaceDetectorYN::create(
        std::string(detection_model_path.begin(), detection_model_path.end()),
        "",                      
        cv::Size(224, 224),
        score_threshold,
        nms_threshold,
        topK
    );

    tensor_input_size = tensor_input_dims[1] * tensor_input_dims[2] * tensor_input_dims[3];
}

void Tracker::predict(cv::Mat& image, FaceData& face_data, const std::unique_ptr<IFilter>& filter)
{
    detect_face(image, face_data); 

    if (face_data.face_detected)
    {
        cv::Point p1(face_data.face_coords[0], face_data.face_coords[1]);
        cv::Point p2(face_data.face_coords[2], face_data.face_coords[3]);
        cv::Mat cropped = image(cv::Rect(p1, p2));

        int height = face_data.face_coords[2] - face_data.face_coords[0];
        int width = face_data.face_coords[3] - face_data.face_coords[1];

        float scale_x = (float)width / 224.0f;
        float scale_y = (float)height / 224.0f;

        detect_landmarks(cropped, face_data.face_coords[0], face_data.face_coords[1], scale_x, scale_y, face_data);

        if (filter != nullptr)
            filter->filter(face_data.landmark_coords, face_data.landmark_coords);

        solver->solve_rotation(&face_data);
    }
}


float inline logit(float p)
{
    if (p >= 0.9999999f)
        p = 0.9999999f; 
    else if (p <= 0.0000001f)
        p = 0.0000001f; 
    
    p = p / (1.0f - p);
    return log(p) / 16.0f;
}

float Tracker::get_distance_squared(float x0, float y0, float x1, float y1)
{
    // calculate distance squared.
    // no need to for sqrt to obtain the smallest distance for optimization
    float x_distance       = (x1 - x0);
    float y_distance       = (y1 - y0);
    float distance_squared = (x_distance * x_distance) + (y_distance * y_distance);
    return distance_squared;
}

int Tracker::get_center_weighted_faces_row(const cv::Mat& image, const cv::Mat& faces)
{
    // get center coordinates for image
    float image_center_x = (float)(image.rows / 2);
    float image_center_y = (float)(image.cols / 2);

    float smallest_distance_squared = 0.0f;
    int   center_weighted_face_row  = -1;
    for (int row = 0; row < faces.rows; row++)
    {
        // get center coordinates for faces at row
        float x0     = faces.at<float>(row, 0);
        float y0     = faces.at<float>(row, 1);
        float face_w = faces.at<float>(row, 2);
        float face_h = faces.at<float>(row, 3);
        float face_center_x = x0 + (face_w / 2);
        float face_center_y = y0 + (face_h / 2);

        float distance_squared = get_distance_squared(image_center_x, image_center_y, face_center_x, face_center_y);
        if ((center_weighted_face_row == -1) || (distance_squared < smallest_distance_squared))
        {
            center_weighted_face_row  = row;
            smallest_distance_squared = distance_squared;
        }
    }
    return center_weighted_face_row;
}

void Tracker::detect_face(const cv::Mat& image, FaceData& face_data)
{
    cv::Mat resized, faces;
    cv::resize(image, resized, cv::Size(224, 224), NULL, NULL, cv::INTER_LINEAR);

    float width = (float)image.cols;
    float height = (float)image.rows;
    int im_width = resized.cols;
    int im_height = resized.rows;

    this->face_detector->detect(resized, faces);

    
    
    // Get data
    face_data.face_detected = false;
    if (faces.rows > 0)
    {
        face_data.face_detected = true;
        int  faces_row = 0;
        bool center_weighted = true; // make center weighted face detection configurable
        if (center_weighted)
            faces_row = get_center_weighted_faces_row(image, faces);
        float x0 = faces.at<float>(faces_row, 0);
        float y0 = faces.at<float>(faces_row, 1);
        float face_w = faces.at<float>(faces_row, 2);
        float face_h = faces.at<float>(faces_row, 3);

        
        float w_ratio = (width / im_width);
        float h_ratio = (height / im_height);

        float x_offset = 0;//face_w * 0.01;
        float y_offset = 0;// face_h * 0.01;

        float face[] = {
            ((x0 - x_offset)  * (w_ratio) ),
            ((y0 - y_offset)  * (h_ratio) ),
            ((face_w)  * (w_ratio) ),
            ((face_h) * (h_ratio) )
        };

        proc_face_detect(face, width, height);

        face_data.face_coords[0] = (int)face[0];
        face_data.face_coords[1] = (int)face[1];
        face_data.face_coords[2] = (int)face[2];
        face_data.face_coords[3] = (int)face[3];
        
    }
}

void Tracker::detect_landmarks(const cv::Mat& image, int x0, int y0, float scale_x, float scale_y, FaceData& face_data)
{
    cv::Mat resized;
    cv::resize(image, resized, cv::Size(224, 224), NULL, NULL, cv::INTER_LINEAR);
    resized.convertTo(resized, CV_32F);
    cv::cvtColor(resized, resized, cv::COLOR_BGR2RGB);
    improc.normalize_and_transpose(resized, buffer_data); // combine methods

    Ort::Value input_tensor = Ort::Value::CreateTensor<float>(memory_info, buffer_data, tensor_input_size, tensor_input_dims, 4);


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

    int additional_width_margin = (int)(w * 0.4f);
    int additional_height_margin = (int)(h * 0.4f);

    int crop_x1 = (int)(x - additional_width_margin);
    int crop_y1 = (int)(y - additional_height_margin);
    int crop_x2 = (int)(x + w + additional_width_margin);
    int crop_y2 = (int)(y + h + additional_height_margin); // force a little taller BB so the chin tends to be covered

    face[0] = (float)std::max(0, crop_x1);
    face[1] = (float)std::max(0, crop_y1);
    face[2] = (float)std::min((int)width, crop_x2);
    face[3] = (float)std::min((int)height, crop_y2);
}


void Tracker::proc_heatmaps(float* heatmaps, int x0, int y0, float scale_x, float scale_y, FaceData& face_data)
{
    int heatmap_size = 784; //28 * 28;
    for (int landmark = 0; landmark < 66; landmark++)
    {
        int offset = heatmap_size * landmark;
        int argmax = -100;
        float maxval = -100;

        float* landmark_heatmap = &heatmaps[offset]; // reduce indexing
        for (int i = 0; i < heatmap_size; i++)
        {
            if (landmark_heatmap[i] > maxval)
            {
                argmax = i;
                maxval = landmark_heatmap[i];
            }
        }

        int x = argmax / 28;
        int y = argmax % 28;


        // float conf = heatmaps[offset + argmax]; unreferenced local variable
        float res = 223;

        int off_x = (int)floor(res * (logit(heatmaps[66 * heatmap_size + offset + argmax])) + 0.1f);
        int off_y = (int)floor(res * (logit(heatmaps[2 * 66 * heatmap_size + offset + argmax])) + 0.1f);


        float lm_x = (float)y0 + (float)(scale_x * (res * (float(x) / 27.0f) + off_x));
        float lm_y = (float)x0 + (float)(scale_y * (res * (float(y) / 27.0f) + off_y));

        face_data.landmark_coords[2 * landmark] = lm_x;
        face_data.landmark_coords[2 * landmark + 1] = lm_y;
    }
}





