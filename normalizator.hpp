#ifndef NORMALIZATOR_HPP
#define NORMALIZATOR_HPP

#include "facedata.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

#define STR_NORMALIZATION_SUCCESS "Face found!"
#define NORMALIZED_WIDTH 100
#define NORMALIZED_HEIGHT 100

class Normalizator
{
	public:
		cv::Mat normalize(FaceData data);
};

#endif