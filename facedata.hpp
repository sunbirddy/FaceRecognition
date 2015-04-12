#ifndef FACEDATA_HPP
#define FACEDATA_HPP

#include "opencv2/core/core.hpp"


//struct is being used to send data between components, notably detector > main > normalizator
struct FaceData
{
	cv::Mat image; 
	cv::Point2f leye, reye; //positions of eyes
	int faceWidth, faceHeight; //size of the face
	FaceData() {};
	FaceData(cv::Mat image_, cv::Point2f leye_, cv::Point2f reye_, int faceWidth_, int faceHeight_):
		image(image_),
		leye(leye_),
		reye(reye_),
		faceWidth(faceWidth_),
		faceHeight(faceHeight_) 
		{
			if(leye.x > reye.x)
			{
				cv::Point swapper = leye;
				leye = reye;
				reye = swapper;
			}
		}
};

#endif
