#include "normalizator.hpp"

Normalizator::Normalizator(cv::Mat image, cv::Point2f eye1, cv::Point2f eye2, int oldWidth, int oldHeight)
{
	face = image;
	leye = eye1;
	reye = eye2;
	faceWidth = oldWidth;
	faceHeight = oldHeight;
	if(leye.x > reye.x)
	{
		cv::Point swapper = leye;
		leye = reye;
		reye = swapper;
	}
}

//shows the normalized face in a new window
void Normalizator::showNormalizedFace()
{
	cv::namedWindow(STR_NORMALIZATION_SUCCESS, cv::WINDOW_AUTOSIZE);
    cv::imshow(STR_NORMALIZATION_SUCCESS, face);
    cv::waitKey(MAX_WAIT_TIME);
    cv::destroyAllWindows();
}

void Normalizator::normalize()
{
	cv::Point2f center = cv::Point2f((float) face.cols / 2, (float) face.rows / 2);
	
	double angle = (double) (leye.y - reye.y) / (leye.x - reye.x);
	angle *= (double) (180 / PI); //conversion from radians into degrees
	//rotation
	cv::Mat trans = cv::getRotationMatrix2D(center, angle, 1.0);
	cv::warpAffine(face, face, trans, cv::Size(face.cols, face.rows));
	
	//cropping image base on the old face coordinates
	face = face(cv::Rect(center.x - faceWidth / 2, center.y - faceHeight / 2, faceWidth, faceHeight)).clone();
	cv::resize(face, face, cv::Size(NORMALIZED_WIDTH, NORMALIZED_HEIGHT));
	cv::imwrite("face.jpg", face);
}