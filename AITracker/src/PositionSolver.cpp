#include "PositionSolver.h"

#define USE_FOV  // Use FOV correction for the camera matrix.

PositionSolver::PositionSolver(
    int width, 
    int height,
    float prior_pitch, 
    float prior_yaw, 
    float prior_distance, 
    bool complex, 
    float fov,
    float x_scale,
    float y_scale, 
    float z_scale) :

    //TODO: Refactor removing prior_yaw/pitch parameters
    landmark_points_buffer(complex ? NB_CONTOUR_POINTS_COMPLEX: NB_CONTOUR_POINTS_BASE, 1, CV_32FC2),
    rv({ 0, 0, 0 }),
    tv({ 0, 0, 0 })
{
    this->prior_pitch = -1.57;
    this->prior_yaw = -1.57;
    this->prior_distance = prior_distance * -2.0;

    this->width = width;
    this->height = height;

    this->rv[0] = this->prior_pitch;
    this->rv[1] = this->prior_yaw;
    this->rv[2] = -1.57;
    this->tv[2] = this->prior_distance;

    //head3dScale = (cv::Mat_<double>(3, 3) <<
    //    y_scale, 0.0, 0,        // pitch is rv[0], pitch involves y-axis
    //    0.0, x_scale, 0,        // yaw is rv[1], yaw involves x-axis
    //    0.0, 0.0, z_scale
    //);


    head3dScale = (cv::Mat_<double>(3, 3) <<
        y_scale, 0.0, 0,        // pitch is rv[0], pitch involves y-axis
        0.0, x_scale, 0,        // yaw is rv[1], yaw involves x-axis
        0.0, 0.0, z_scale
        );

    this->complex = complex;

    if (!complex)
    { 
        contour_indices = { 0,1,8,15,16,27,28,29,30,31,32,33,34,35,36,39,42,45 };
        mat3dcontour = (cv::Mat_<double>(NB_CONTOUR_POINTS_BASE, 3) <<
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
    }
    else
    {
        contour_indices = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,27,28,29,30,31,32,33,34,35,36,39,42,45 };

        landmark_points_buffer = cv::Mat((int)contour_indices.size(), 1, CV_32FC2);

        mat3dcontour = (cv::Mat_<double>((int)contour_indices.size(), 3) <<
            0.45517698, -0.30089578, 0.76442945,        //0
            0.44899884, -0.16699584, 0.76514298,        //1
            0.43743154, -0.02265548, 0.73926717,        
            0.41503343, 0.08894145, 0.74794745,
            0.38912359, 0.23238003, 0.70478839,
            0.3346301, 0.36126539, 0.61558759,
            0.2637251, 0.46000972, 0.49147922,
            0.16241622, 0.55803716, 0.33944517,
            0., 0.62107903, 0.28729478,
            -0.16241622, 0.55803716, 0.33944517,
            -0.2637251, 0.46000972, 0.49147922,
            -0.3346301, 0.36126539, 0.61558759,
            -0.38912359, 0.23238003, 0.70478839,
            -0.41503343, 0.08894145, 0.74794745,
            -0.43743154, -0.02265548, 0.73926717,
            -0.44899884, -0.16699584, 0.76514298,
            0., -0.29333261, 0.13758209,
            0., -0.1948287, 0.06915811,
            0., -0.10384402, 0.00915182,
            0., 0., 0.,
            0.08062635, 0.04127607, 0.13416104,
            0.04643935, 0.05767522, 0.10299063,
            0., 0.06875312, 0.09054535,
            -0.04643935, 0.05767522, 0.10299063,
            -0.08062635, 0.04127607, 0.13416104,
            0.31590518, -0.29833749, 0.2851074,
            0.13122973, -0.28444737, 0.23423915,
            -0.13122973, -0.28444737, 0.23423915,
            -0.31590518, -0.29833749, 0.2851074
            );
    }

    // Taken from 
    // https://github.com/opentrack/opentrack/blob/3cc3ef246ad71c463c8952bcc96984b25d85b516/tracker-aruco/ftnoir_tracker_aruco.cpp#L193
    // Taking into account the camera FOV instead of assuming raw image dims is more clever and
    // will make the solver more camera-agnostic.
    float diag_fov = (float)(fov * TO_RAD);

    // Get expressed in sensor-size units

 #ifdef USE_FOV
    // field of view is a rectangular viewport with corners on a circular lens
    // the diagonal of the rectangle can be expressed as the angular field of view or pixels
    // the width of the rectangle can be expressed as either the field of view width or width in pixels
    // the height of the rectangle can be expressed as either the field of view height or height in pixes
    double width_squared    = (double)width * width;
    double height_squared   = (double)height * height;
    double diagonal_squared = width_squared + height_squared;
    double diagonal         = sqrt(diagonal_squared); // hypotenuse of triangle

    // set default focalLength for width and heigh if field of view is not set
    double focalLength_width = width;
    double focalLength_height = height;
    if (fov != 0.0)
    {
        double fov_w = (double)diag_fov * width / diagonal;
        double fov_h = (double)diag_fov * height / diagonal;

        focalLength_width = 0.5 * width / tan(0.5 * fov_w);
        focalLength_height = 0.5 * height / tan(0.5 * fov_h);
    }
 
    camera_matrix = (cv::Mat_<double>(3, 3) <<
        focalLength_height, 0, height / 2,
        0, focalLength_width, width / 2,
        0, 0, 1
        );

 #else
    double fov_w = 2. * atan(tan(diag_fov / 2.) / sqrt(1. + height / (double)width * height / (double)width));
    double fov_h = 2. * atan(tan(diag_fov / 2.) / sqrt(1. + width / (double)height * width / (double)height));

    float i_height = (float)(0.5f * height / (tan(0.5 * fov_w)));
    float i_width = (float)(0.5f * width / (tan(0.5 * fov_h)));

    /*camera_matrix = (cv::Mat_<double>(3, 3) <<
        height, 0, height / 2,
        0, height, width / 2,
        0, 0, 1
        );*/

    camera_matrix = (cv::Mat_<double>(3, 3) <<
        i_width, 0, height / 2,
        0, i_height, width / 2,
        0, 0, 1
    );


 #endif

    camera_distortion = (cv::Mat_<double>(4, 1) << 0, 0, 0, 0);

    //mat3dcontour =  mat3dcontour * head3dScale;

    cv::transpose(mat3dcontour, mat3dcontour);
    mat3dcontour = head3dScale * mat3dcontour;
    cv::transpose(mat3dcontour, mat3dcontour);

    if(complex) std::cout << "Using complex solver" << std::endl;
}


void PositionSolver::solve_rotation(FaceData* face_data)
{
    int contour_idx = 0;
    for (int j = 0; j < 2; j++)
    {
        for (int i = 0; i < contour_indices.size(); i++)
        {
            contour_idx = contour_indices[i];
            landmark_points_buffer.at<float>(i, j) = (float)face_data->landmark_coords[2 * contour_idx + j]; // fix complation warnings.
        }
    }

    cv::Mat rvec(rv, true), tvec(tv, true);


   solvePnP(mat3dcontour,
        landmark_points_buffer,
        this->camera_matrix,
        this->camera_distortion,
        rvec,
        tvec,
        !this->complex, //extrinsic guess
        cv::SOLVEPNP_ITERATIVE
    );

    get_euler(rvec, tvec);


    for (int i = 0; i < 3; i++)
    {
        face_data->rotation[i] = rvec.at<double>(i, 0);
        face_data->translation[i] = tvec.at<double>(i, 0) * 10; // scale solvePnP coordinates to opentrack units in centimeters
    }

    correct_rotation(*face_data);
    //clip_rotations(*face_data);

#ifdef _DEBUG
    std::cout << face_data->to_string() << std::endl; // disable copy constructor and output to std::cout
#endif

}

void PositionSolver::set_prior_pitch(float new_pitch)
{
    this->prior_pitch = new_pitch;
    this->rv[0] = this->prior_pitch;
}

void PositionSolver::set_prior_yaw(float new_yaw)
{
    this->prior_yaw = new_yaw;
    this->rv[1] = this->prior_yaw;
}

void PositionSolver::set_prior_distance(float new_distance)
{
    this->prior_distance = new_distance * -2.;
    this->tv[2] = this->prior_distance;
}

void PositionSolver::calibrate_head_scale(FaceData& face_data)
{
    std::tuple<double, double> face_dims = get_3dhead_dims();
    double width = std::get<0>(face_dims);
    double height = std::get<1>(face_dims);

    double real_ratio = width / height;

    std::tuple<double, double> model_dims = get_3dhead_dims();
    double model_width = std::get<0>(model_dims);
    double model_height = std::get<1>(model_dims);

    double model_ratio = model_width / model_height;

    double scale = (model_height * real_ratio) / height;

    head3dScale.at<double>(0, 0) = scale;
}

double PositionSolver::get_x_scale()
{
    return head3dScale.at<double>(1, 1);
}

std::tuple<double, double> PositionSolver::get_3dhead_dims()
{
    // indices of the matrix rows, not actual points!
    double model_width = abs(mat3dcontour.at<double>(0, 0) - mat3dcontour.at<double>(8, 0));   // 0 - 16
    double model_height = abs(mat3dcontour.at<double>(9, 1) - mat3dcontour.at<double>(4, 1));  // 27 -8
    return std::tuple<double, double>(model_width, model_height);
}

std::tuple<double, double> PositionSolver::get_2dhead_dims(FaceData& face_data)
{
    double width = abs(face_data.landmark_coords[0 + 1] - face_data.landmark_coords[16 * 2 + 1]);
    double height = abs(face_data.landmark_coords[27 * 2 + 0] - face_data.landmark_coords[8 * 2 + 0]);
    return std::tuple<double, double>(width, height);
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

void PositionSolver::correct_rotation(FaceData& face_data)
{
    float distance = (float) abs(face_data.translation[2]);
    float lateral_offset = (float)face_data.translation[1];
    float verical_offset = (float)face_data.translation[0];

    //float correction_yaw = (float)std::atan((distance / abs(lateral_offset))) * TO_DEG;
    //float correction_pitch = (float)(distance / std::atan(verical_offset) * TO_DEG);


    float correction_yaw = 90.0f - (float)std::atan2(distance, abs(lateral_offset)) * TO_DEG;
    float correction_pitch = 90.0f - (float)std::atan2(distance, abs(verical_offset)) * TO_DEG;

    if (lateral_offset < 0)
        correction_yaw *= -1;

    if (verical_offset < 0)
        correction_pitch *= -1;

    face_data.rotation[1] += correction_yaw;
    face_data.rotation[0] += correction_pitch;

    // Note: We could saturate pitch here, but its better to let the user do it via Opentrack.
    // The coefficient could be problematic for some users.
    //face_data.rotation[0] = face_data.rotation[0] * 1.5;

    // We dont want the Z axis oversaturated since opentrack has +/-600 centimeter range
    face_data.translation[2] /= 100;
}


void PositionSolver::clip_rotations(FaceData& face_data)
{
    // Limit yaw between -90.0 and +90.0 degrees
    if (face_data.rotation[1] >= 90.0)
        face_data.rotation[1] = 90.0;
    else if (face_data.rotation[1] <= -90.0)
        face_data.rotation[1] = -90.0;
    // Limit pitch between -90.0 and +90.0 
    if (face_data.rotation[0] >= 90.0)
        face_data.rotation[0] = 90.0;
    else if (face_data.rotation[0] <= -90.0)
        face_data.rotation[0] = -90.0;
    // Limit roll between 0.0 and +180.0 
    if (face_data.rotation[2] >= 180.0)
        face_data.rotation[2] = 180.0;
    else if (face_data.rotation[2] <= 0.0)
        face_data.rotation[2] = 0.0;
}



/*
*   SIMPLE POSITION SOLVER
*/
SimplePositionSolver::SimplePositionSolver(int im_width, int im_height, float prior_pitch, float prior_yaw, float prior_distance, bool complex, float fov, float x_scale, float y_scale, float z_scale):
   PositionSolver(im_width, im_height,prior_pitch, prior_yaw, prior_distance, complex, fov, x_scale, y_scale, z_scale)
{
    contour_indices = { 0,1,2,3,8,13,14,15,16,27,28,29,30,39,42,55 };

    landmark_points_buffer = cv::Mat((int)contour_indices.size(), 1, CV_32FC2);

    mat3dcontour = (cv::Mat_<double>((int)contour_indices.size(), 3) <<
        0.4551769692672, 0.300895790030204, -0.764429433974752,
        0.448998827123556, 0.166995837790733, -0.765143004071253,
        0.437431554952677, 0.022655479179981, -0.739267175112735,
        0.415033422928434, -0.088941454648772, -0.747947437846473,
        0., -0.621079019321682, -0.287294770748887,
        -0.415033422928434, -0.088941454648772, -0.747947437846473,
        -0.437431554952677, 0.022655479179981, -0.739267175112735,
        -0.448998827123556, 0.166995837790733, -0.765143004071253,
        -0.4551769692672, 0.300895790030204, -0.764429433974752,
        0., 0.293332603215811, -0.137582088779393,
        0., 0.194828701837823, -0.069158109325951,
        0., 0.103844017393155, -0.009151819844964,
        0., 0., 0.,
        0.131229723798772, 0.284447361805627, -0.234239149487417,
        -0.131229723798772, 0.284447361805627, -0.234239149487417,
        0., -0.343742581679188, -0.113925986025684
        );

    // This 3d model is "inverted", so we need to also invert scales
    head3dScale = (cv::Mat_<double>(3, 3) <<
        y_scale, 0.0, 0,        // pitch is rv[0], pitch involves y-axis
        0.0, x_scale, 0,        // yaw is rv[1], yaw involves x-axis
        0.0, 0.0, z_scale
        );

    cv::transpose(mat3dcontour, mat3dcontour);
    mat3dcontour = head3dScale * mat3dcontour;
    cv::transpose(mat3dcontour, mat3dcontour);
}



std::tuple<double, double> SimplePositionSolver::get_3dhead_dims()
{
    // indices of the matrix rows, not actual points!
    double model_width = abs(mat3dcontour.at<double>(0, 0) - mat3dcontour.at<double>(8, 0));   // 0 - 16
    double model_height = abs(mat3dcontour.at<double>(9, 1) - mat3dcontour.at<double>(4, 1));  // 27 -8
    return std::tuple<double, double>(model_width, model_height);
}

std::tuple<double, double> SimplePositionSolver::get_2dhead_dims(FaceData& face_data)
{
    double width = abs(face_data.landmark_coords[0 + 1] - face_data.landmark_coords[16 * 2 + 1]);
    double height = abs(face_data.landmark_coords[27 * 2 + 0] - face_data.landmark_coords[8 * 2 + 0]);
    return std::tuple<double, double>(width, height);
}


void SimplePositionSolver::correct_rotation(FaceData& face_data)
{
    // For some reason the solver gets rest "Pitch" as "-180 deg", which is the same 
    // as "0 deg", which is what the other solvers find.
    // For the moment this method will be overriden beacuse it's very possible that 
    // this simpler model will change (soon). 

    if(abs(face_data.rotation[0]) > 90)
        if (face_data.rotation[0] >= -180 && face_data.rotation[0] <= 0) {
            face_data.rotation[0] += 180;
        }
        else if (face_data.rotation[0] > 0 && face_data.rotation[0] <= 180)
        {
            face_data.rotation[0] -= 180;
        }

    PositionSolver::correct_rotation(face_data);
}