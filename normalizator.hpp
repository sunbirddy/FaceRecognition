#ifndef NORMALIZATOR_HPP
#define NORMALIZATOR_HPP

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#define PI 3.1416
#define STR_NORMALIZATION_SUCCESS "Face found!"
#define NORMALIZED_WIDTH 100
#define NORMALIZED_HEIGHT 100
#define MAX_WAIT_TIME 60000

class Normalizator
{
	private:
		cv::Mat face;
		cv::Point2f leye, reye;
		int faceWidth, faceHeight;
	public:
		Normalizator(cv::Mat image, cv::Point2f leye, cv::Point2f reye, int oldWidth, int oldHeight);
		void showNormalizedImage();
		void showNormalizedWebcam();
		void normalize();
};

#endif