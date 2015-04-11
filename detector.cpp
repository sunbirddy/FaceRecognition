#include "detector.hpp"

/*
Detector::Detector(char* path)
{
	imgPath = path;
}
*/

inline int min(int a, int b)
{
	return a < b ? a : b;
}

Detector::Detector()
{
	if(!face_cascade.load("/usr/share/opencv/haarcascades/haarcascade_frontalface_alt.xml")) {
		puts("Could not load cascade file for face. Application terminated.");
	}
	if(!eye_cascade.load("/usr/share/opencv/haarcascades/haarcascade_eye.xml")) {
		puts("Could not load cascade file for eye. Application terminated.");
	}
}

//reads image from file
//TODO: add reading images from webcam
bool Detector::getImage()
{
	image = cv::imread("image3.jpg", 0);
	return image.data;
}

//finds faces and eyes in the given image
void Detector::fetchFace()
{
	faces.clear();
	eyes.clear();
	face_cascade.detectMultiScale(image, faces, HAAR_SCALE_FAC_PIC, 
		HAAR_MIN_NEIGH_PIC, HAAR_FLAGS_PIC, cv::Size(HAAR_MIN_FACE_SIZE, HAAR_MIN_FACE_SIZE));

	//for(int i=0; i<faces.size(); i++)
	//	rectangle(image, cv::Point(faces[i].x, faces[i].y), cv::Point(faces[i].x + faces[i].width, faces[i].y + faces[i].height), cv::Scalar(0));

	//in case we found a face now we need to find the eyes
	if(!faces.empty())
	{
		//finding the biggest face found (highest chance of actually getting a face)
		cv::Rect biggestFace = faces[0];
		for(int i = 1; i < faces.size(); i++)
			if(faces[i].area() > biggestFace.area())
				biggestFace = faces[i];

		//we only need to look for eyes in the face rectangle, so we set the ROI
		cv::Mat face = image(biggestFace);

		//searching for eyes
		eye_cascade.detectMultiScale(face, eyes, HAAR_SCALE_FAC_PIC, 
			HAAR_MIN_NEIGH_PIC, HAAR_FLAGS_PIC, cv::Size(HAAR_MIN_EYE_SIZE, HAAR_MIN_EYE_SIZE));

		//now we should send to the normalizator a little more than the rectangle that contains face, because after
		//rotation (e.g. 45 degrees) we might lose a lot of the face in order for the image to remain rectangular
		//thus, we calculate how much more do we need and add it to our face-rectangle
		int diagonal = ceil(sqrt(biggestFace.width * biggestFace.width + biggestFace.height * biggestFace.height));
		int diffWidth, diffHeight; //how much have we moved the x and y of biggest face (will be needed for adjusting eyes)
		diffWidth = min((diagonal - biggestFace.width) / 2, biggestFace.x);
		diffHeight = min((diagonal - biggestFace.height) / 2, biggestFace.y);
		biggestFace.x = biggestFace.x - diffWidth;
		biggestFace.y = biggestFace.y - diffHeight;
		biggestFace.width = min(diagonal, image.cols);
		biggestFace.height = min(diagonal, image.rows);

		//now we can get rid of the rest of the image since face is all we need
		image = image(cv::Rect(biggestFace.x, biggestFace.y, biggestFace.width, biggestFace.height)).clone();

		for(int i = 0; i < eyes.size(); i++)
		{
			//need to adjust the eyes' position to the bigger ROI
			eyes[i].x += diffWidth;
			eyes[i].y += diffHeight;
			rectangle(image, cv::Point(eyes[i].x, eyes[i].y), cv::Point(eyes[i].x + eyes[i].width, eyes[i].y + eyes[i].height), cv::Scalar(0));
		}

		if(eyes.size() > 1) //we need at least two eyes
		{
			cv::Point2f leye = cv::Point2f((float) eyes[0].x + eyes[0].width / 2, (float) eyes[0].y + eyes[0].height / 2); //middle of the left...
			cv::Point2f reye = cv::Point2f((float) eyes[1].x + eyes[1].width / 2, (float) eyes[1].y + eyes[1].height / 2); //...and the right eye

			printf("%d %d %d %d\n%d %d %d %d\n", eyes[0].x, eyes[0].y, eyes[0].width, eyes[0].height, eyes[1].x, eyes[1].y, eyes[1].width, eyes[1].height);
			printf("%f %f %f %f\n", leye.x, leye.y, reye.x, reye.y);
			Normalizator norm = Normalizator(image, leye, reye);
			norm.normalize();
			norm.showNormalizedFace();
		}
	}
}

//returns false if detector is good to go, otherwise returns true
bool Detector::isFaulty()
{
	return face_cascade.empty() || eye_cascade.empty();
}