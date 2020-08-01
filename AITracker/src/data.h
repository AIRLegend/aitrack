#pragma once
#include <string>

class FaceData
{
public:
	// Yaw, Pitch, Roll
	double* rotation;

	// X, Y, Z
	double* translation;

	// Landmark positions: [[x,y], [x,y], [x,y], ...]
	float landmark_coords[66 * 2]; 

	// Crop position x0, y0, x1, y1
	int* face_coords;

	bool face_detected;

	FaceData();

	std::string to_string();
};

