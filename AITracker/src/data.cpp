#include "data.h"

FaceData::FaceData():
	landmark_coords{0}
{
	rotation = new double[3]{ 0,0,0 };
	translation = new double[3]{ 0,0,0 };
	face_coords = new int[4]{ 0,0,0,0 };
	face_detected = false;
}

std::string FaceData::to_string()
{
	std::string datastring =
		std::string("Pitch ") + std::to_string(this->rotation[0]) +
		std::string("  Yaw ") + std::to_string(this->rotation[1]) +
		std::string("  Roll ") + std::to_string(this->rotation[2]);

	datastring +=
		std::string("   X: ") +
		std::to_string(this->translation[0]) + ", Y: " +
		std::to_string(this->translation[1]) + ", Z: " +
		std::to_string(this->translation[2]);

	return datastring;
}


