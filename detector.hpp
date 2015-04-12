#ifndef DETECTOR_HPP
#define DETECTOR_HPP

#include "normalizator.hpp"
#include <vector>
#include <cstdio>
#include <cmath>
#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#define STR_READ_FAILURE "Could not read the image"
#define STR_FACE_NFOUND "No face found in the picture"
#define STR_EYES_NFOUND "No eyes found in the face"
#define HAAR_MIN_FACE_SIZE 10
#define HAAR_MIN_EYE_SIZE 5
#define HAAR_SCALE_FAC_PIC 1.1
#define HAAR_MIN_NEIGH_PIC 3
#define HAAR_FLAGS_PIC 0
#define HAAR_SCALE_FAC_CAM 1.2
#define HAAR_MIN_NEIGH_CAM 2
#define HAAR_FLAGS_CAM cv::CV_HAAR_DO_CANNY_PRUNING

class Detector
{
	private:
		cv::CascadeClassifier face_cascade, eye_cascade;
		std::vector <cv::Rect> faces, eyes;
		cv::Mat image;
	public:
		Detector();
		bool getImage(char * path);
		bool fetchFace();
		bool isFaulty();
};

#endif