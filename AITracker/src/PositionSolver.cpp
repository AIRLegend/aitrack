#include "PositionSolver.h"


PositionSolver::PositionSolver(int width, int height,
    float prior_pitch, float prior_yaw, float prior_distance) :
    contour_indices{ 0,1,8,15,16,27,28,29,30,31,32,33,34,35,36,39,42,45 },
    landmark_points_buffer(18, 1, CV_32FC2),
    rv({ 0, 0, 0 }),
    tv({ 0, 0, 0 })
{
    this->prior_pitch = (1.1f * (prior_pitch + 90.f) / 180.f) - (double)2.5f;
    this->prior_distance = prior_distance * -2.;
    this->prior_yaw = (1.84f * (prior_yaw + 90.f) / 180.f) - (double)3.14f;

    this->rv[0] = this->prior_pitch;
    this->rv[1] = this->prior_yaw;
    this->tv[2] = this->prior_distance;

    //std::cout << "PRIORS CALCULATED: \nPITCH: " <<this->prior_pitch << "  YAW: " << this->prior_yaw << "  DISTANCE: " << this->prior_distance;

    mat3dcontour = (cv::Mat_<double>(18, 3) <<
        0.45517698, -0.30089578, 0.76442945,
        0.44899884, -0.16699584, 0.765143,
        0., 0.621079, 0.28729478,
        -0.44899884, -0.16699584, 0.765143,
        -0.45517698, -0.30089578, 0.76442945,
        0., -0.2933326, 0.1375821,
        0., -0.1948287, 0.06915811,
        0., -0.10384402, 0.00915182,
        0., 0., 0.,
        0.08062635, 0.04127607, 0.13416104,
        0.04643935, 0.05767522, 0.10299063,
        0., 0.06875312, 0.09054535,
        -0.04643935, 0.05767522, 0.10299063,
        -0.08062635, 0.04127607, 0.13416104,
        0.31590518, -0.2983375, 0.2851074,
        0.13122973, -0.28444737, 0.23423915,
        -0.13122973, -0.28444737, 0.23423915,
        -0.31590518, -0.2983375, 0.2851074
        );

    camera_matrix = (cv::Mat_<double>(3, 3) <<
        height, 0, height / 2,
        0, height, width / 2,
        0, 0, 1
        );

    camera_distortion = (cv::Mat_<double>(4, 1) << 0, 0, 0, 0);
}

void PositionSolver::solve_rotation(FaceData* face_data)
{
    int contour_idx = 0;
    for (int j = 0; j < 2; j++)
    {
        for (int i = 0; i < NB_CONTOUR_POINTS; i++)
        {
            contour_idx = contour_indices[i];
            landmark_points_buffer.at<float>(i, j) = (int)face_data->landmark_coords[2 * contour_idx + j];

        }
    }

    cv::Mat rvec(rv, true), tvec(tv, true);

    solvePnP(mat3dcontour,
        landmark_points_buffer,
        this->camera_matrix,
        this->camera_distortion,
        rvec,
        tvec,
        true, //extrinsic guess
        cv::SOLVEPNP_ITERATIVE
    );

    get_euler(rvec, tvec);


    for (int i = 0; i < 3; i++)
    {
        face_data->rotation[i] = rvec.at<double>(i, 0);
        face_data->translation[i] = tvec.at<double>(i, 0);
    }

}


void PositionSolver::get_euler(cv::Mat& rvec, cv::Mat& tvec)
{
    cv::Mat rotM(3, 3, CV_64FC1);
    cv::Rodrigues(rvec, rotM);

    cv::Mat concated(3, 4, CV_64FC1);
    cv::hconcat(rotM, tvec, concated);

    cv::decomposeProjectionMatrix(
        concated,
        cv::Mat(3, 3, CV_64FC1),
        cv::Mat(3, 3, CV_64FC1),
        cv::Mat(4, 1, CV_64FC1),
        cv::noArray(),
        cv::noArray(),
        cv::noArray(),
        rvec
    );

}

