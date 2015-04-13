#include "normalizator.hpp"

//normalizes the face, that is rescales the image to NORMALIZED_WIDTH x NORMALIZED_HEIGHT and
//puts eyes in (25,25), (75, 25)
cv::Mat Normalizator::normalize(FaceData data)
{
	cv::Mat face = data.image;
	 //rotation angle in RADIANS
	double angle = atan2((double) (data.leye.y - data.reye.y), (double) (data.leye.x - data.reye.x)) - CV_PI;
	//rotation, conversion to degrees needed
	cv::Mat trans = cv::getRotationMatrix2D(data.leye, (double) (180 / CV_PI) * angle, 1.0); 
	cv::warpAffine(face, face, trans, cv::Size(face.cols, face.rows));

	//now we need to calculate the new eyes' positions
	//the goal is much easier to achieve if we use coordinates related to faceCenter
	data.leye.x -= data.faceCenter.x; data.leye.y -= data.faceCenter.y;
	data.reye.x -= data.faceCenter.x; data.reye.y -= data.faceCenter.y;
	data.leye.y = -data.leye.y; data.reye.y = -data.reye.y; //converting to xy-Cartesian plane coordinates
	float sina = sin(angle), cosa = cos(angle);

	//we use the formula for new coordinates after spinning by a: (x, y) -> (xcosa - ysina, xsina + ycosa)
	data.leye = cv::Point2f(data.leye.x * cosa - data.leye.y * sina, data.leye.x * sina + data.leye.y * cosa);
	data.reye = cv::Point2f(data.reye.x * cosa - data.reye.y * sina, data.reye.x * sina + data.reye.y * cosa);

	//back to absolute coordinates
	data.leye.y = -data.leye.y; data.reye.y = -data.reye.y;
	data.leye.x += data.faceCenter.x; data.leye.y += data.faceCenter.y;
	data.reye.x += data.faceCenter.x; data.reye.y += data.faceCenter.y;
	//since the positions eyes should be in are (25, 25) and (75, 25) in a 100x100 image, 
	//we know the differnce between their x coordinate is half of the output image width desired
	int width = 2 * (data.reye.x - data.leye.x);
	cv::Rect output;
	//mins and maxes are guardians so we wouldnt go out of range
	output.x = std::max(0, (int) data.leye.x - width / 4); //if we have too little scratch of a face, so be it
	output.y = std::max(0, (int) data.leye.y - face.rows / 4); //we cannot get the rest of it out of thin air!
	output.width = std::min(face.cols - output.x, width);
	output.height = std::min(face.rows - output.y, (int) data.leye.y + (3 * face.rows) / 4);
	//cropping image based on the old face coordinates
	face = face(output).clone();
	cv::resize(face, face, cv::Size(NORMALIZED_WIDTH, NORMALIZED_HEIGHT));
	return face;
}