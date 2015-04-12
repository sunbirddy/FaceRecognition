#ifndef DETECTOR_HPP
#define DETECTOR_HPP

#include "normalizator.hpp"
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#define HAAR_FACE_PATH "cascades/haarcascade_frontalface_alt.xml"
#define HAAR_EYE_PATH "cascades/haarcascade_eye.xml"
#define STR_NO_FACE_CASC "Could not load cascade file for face\n"
#define STR_NO_EYE_CASC "Could not load cascade file for eyes\n"
#define STR_READ_FAILURE "Could not read the image\n"
#define STR_FACE_NFOUND "No face found in the picture\n"
#define STR_EYES_NFOUND "No eyes found in the face\n"
#define HAAR_MIN_FACE_SIZE 75
#define HAAR_FACE_SEARCH_DIV 10
#define HAAR_EYE_SEARCH_DIV 10
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
		cv::Mat image;
	public:
		Detector();
		inline cv::Size minFaceSize(int cols, int rows);
		bool getImage(std::string path);
		bool fetchFace();
		void runCamera();
		bool isFaulty();
};

#endif