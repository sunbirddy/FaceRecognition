#ifndef DETECTOR_HPP
#define DETECTOR_HPP

#define DEBUG
#include "normalizator.hpp"
#include "facedata.hpp"
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cmath>
#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#ifdef DEBUG
	#include <iostream>
	#include "opencv2/highgui/highgui.hpp"
#endif

#define HAAR_FACE_PATH "cascades/haarcascade_frontalface_alt.xml"
#define HAAR_EYE_PATH "cascades/haarcascade_eye.xml"
#define STR_NO_FACE_CASC "Could not load cascade file for face\n"
#define STR_NO_EYE_CASC "Could not load cascade file for eyes\n"
#define STR_FACE_NFOUND "No face found in the picture\n"
#define STR_EYES_NFOUND "No eyes found in the face\n"
#define ARGS_PIC 0
#define ARGS_CAM 1
#define HAAR_MIN_FACE_SIZE 75
#define HAAR_FACE_SEARCH_DIV 10
#define HAAR_EYE_SEARCH_DIV 5
#define HAAR_SCALE_FAC_PIC 1.1
#define HAAR_MIN_NEIGH_PIC 3
#define HAAR_FLAGS_PIC 0
#define HAAR_SCALE_FAC_CAM 1.2
#define HAAR_MIN_NEIGH_CAM 2
#define HAAR_FLAGS_CAM CV_HAAR_DO_CANNY_PRUNING

class Detector
{
	private:
		float scaleFac; //this...
		int minNeigh, flags; //...and this are arguments for searching faces / eyes
		cv::CascadeClassifier face_cascade, eye_cascade;
	public:
		Detector() : //by default the arguments are set for single-image search
			scaleFac(HAAR_SCALE_FAC_PIC),
			minNeigh(HAAR_MIN_NEIGH_PIC),
			flags(HAAR_FLAGS_PIC) {}
		void init();
		void setArgumentsMan(float scaleFac_, int minNeigh_, int flags_);
		void setArguments(int flag_);
		inline cv::Size minFaceSize(int cols, int rows);
		FaceData fetchFaceAndEyes(cv::Mat image);
};

#endif