#include "data.h"

FaceData::FaceData()
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
		std::string("\n\Crop coords: ") + 
		std::to_string(this->face_coords[0]) + ", " + 
		std::to_string(this->face_coords[1]) + ", " +
		std::to_string(this->face_coords[2]) + ", " + 
		std::to_string(this->face_coords[3]);

	return datastring;
}


