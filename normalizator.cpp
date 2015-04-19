#include "normalizator.hpp"

//normalizes the face, that is rescales the image to NORMALIZED_WIDTH x NORMALIZED_HEIGHT and
//puts eyes in (25,25), (75, 25)
cv::Mat Normalizator::normalize(FaceData data)
{
	cv::Mat face = data.image;

	double angle = atan2((double) (data.leye.y - data.reye.y), (double) (data.leye.x - data.reye.x)) - CV_PI;
	angle *= (double) (180 / CV_PI); //converting radians into degrees
	//rotation
	cv::Mat trans = cv::getRotationMatrix2D(data.leye, angle, 1.0);
	cv::warpAffine(face, face, trans, cv::Size(face.cols, face.rows));

	//since the positions eyes should be in are (25, 25) and (75, 25) in a 100x100 image, 
	//we know the differnce between their x coordinate is half of the output image width desired
	int side = 2 * (data.reye.x - data.leye.x);
	cv::Rect output;
	//mins and maxes are guardians so we wouldnt go out of range
	output.x = std::max(0, (int) data.leye.x - side / 4); //if we have too little scratch of a face, so be it,...
	output.y = std::max(0, (int) data.leye.y - side / 4); //...we cannot get the rest of it out of thin air!
	output.width = std::min(side, face.cols - output.x);
	output.height = std::min(side, face.rows - output.y);
	
	//cropping image
	face = face(output);
	cv::resize(face, face, cv::Size(NORMALIZED_WIDTH, NORMALIZED_HEIGHT));
	return face;
}