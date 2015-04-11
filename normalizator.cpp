#include "normalizator.hpp"

Normalizator::Normalizator(cv::Mat image, cv::Point2f eye1, cv::Point2f eye2)
{
	face = image;
	leye = eye1;
	reye = eye2;
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
    cv::waitKey(0);
}

void Normalizator::normalize()
{
	cv::Point2f center = cv::Point2f((float) face.cols / 2, (float) face.rows / 2);
	
	/*
	cv::Point2f middle = cv::Point2f((float) (leye.x + reye.x) / 2, (float) (leye.y + reye.y) / 2);

	cv::Point2f newleye = cv::Point2f((float) face.cols / 4, (float) face.rows / 4);
	cv::Point2f newreye = cv::Point2f((float) face.cols * 3 / 4, (float) face.rows / 4);

	cv::Point2f newmiddle = cv::Point2f((float) (newleye.x + newreye.x) / 2, (float) (newleye.y + newreye.y) / 2);
	cv::Point2f beforeTrans[] = {middle, leye, reye};
	cv::Point2f afterTrans[] = {newmiddle, newleye, newreye};
	cv::Mat trans = cv::getAffineTransform(beforeTrans, afterTrans);
	*/
	double angle = (double) (leye.y - reye.y) / (leye.x - reye.x);
	cv::Mat trans = cv::getRotationMatrix2D(center, double(180 / PI) * atan(angle), 1.0);
	cv::warpAffine(face, face, trans, cv::Size(face.cols, face.rows));
}