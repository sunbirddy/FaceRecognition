#ifndef NORMALIZATOR_HPP
#define NORMALIZATOR_HPP

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#define PI 3.1416
#define STR_NORMALIZATION_SUCCESS "Face found!"

class Normalizator
{
	private:
		cv::Mat face;
		cv::Point2f leye, reye;
	public:
		Normalizator(cv::Mat image, cv::Point2f leye, cv::Point2f reye);
		void showNormalizedFace();
		void normalize();
};

#endif