#ifndef DETECTOR_HPP
#define DETECTOR_HPP

#include "normalizator.hpp"
#include <vector>
#include <cstdio>
#include <cmath>
#include "opencv2/core/core.hpp"
//#include "opencv2/highgui/highgui.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/objdetect/objdetect.hpp"
//#include "opencv2/imgproc/imgproc.hpp"

#define STR_NORMALIZATION_SUCCESS "Face found!"
#define STR_READ_FAILURE "Could not read the image"
#define HAAR_MIN_FACE_SIZE 10
#define HAAR_SCALE_FAC_PIC 1.1
#define HAAR_MIN_NEIGH_PIC 3
#define HAAR_FLAGS_PIC 0
#define HAAR_SCALE_FAC_CAM 1.2
#define HAAR_MIN_NEIGH_CAM 2
#define HAAR_FLAGS_CAM cv::CV_HAAR_DO_CANNY_PRUNING
#define HAAR_MIN_EYE_SIZE 6

class Detector
{
	private:
		cv::CascadeClassifier face_cascade, eye_cascade;
		std::vector <cv::Rect> faces, eyes;
		cv::Mat image;
		char * imgPath; //path we read image from (not implemented yet)
	public:
		Detector();
		bool getImage();
		void fetchFace();
		bool isFaulty();
};

#endif