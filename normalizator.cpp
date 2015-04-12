#include "normalizator.hpp"

//normalizes the face, that is rescales the image to NORMALIZED_WIDTH x NORMALIZED_HEIGHT and
//puts eyes in (25,25), (75, 25)
cv::Mat Normalizator::normalize(FaceData data)
{
	cv::Mat face = data.image;

	cv::Point2f center = cv::Point2f((float) face.cols / 2, (float) face.rows / 2);
	
	double angle = atan2((double) (data.leye.y - data.reye.y), (double) (data.leye.x - data.reye.x)) - CV_PI;
	angle *= (double) (180 / CV_PI); //converting radians into degrees
	//rotation
	cv::Mat trans = cv::getRotationMatrix2D(center, angle, 1.0);
	cv::warpAffine(face, face, trans, cv::Size(face.cols, face.rows));
	
	//cropping image based on the old face coordinates
	face = face(cv::Rect(center.x - data.faceWidth / 2, center.y - data.faceHeight / 2, data.faceWidth, data.faceHeight)).clone();
	cv::resize(face, face, cv::Size(NORMALIZED_WIDTH, NORMALIZED_HEIGHT));
	return face;
}