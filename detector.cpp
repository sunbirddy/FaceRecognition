#include "detector.hpp"

/*
Detector::Detector(char* path)
{
	imgPath = path;
}
*/

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
	image = cv::imread("image.jpg", 0);
	return image.data;
}

//shows image in a new window
//TODO: copy code to normalizator and delete this
void Detector::showImage()
{
	cv::namedWindow(STR_FETCH_SUCCESS, cv::WINDOW_AUTOSIZE);
    cv::imshow(STR_FETCH_SUCCESS, image);
    cv::waitKey(0);
}

//finds faces and eyes in the given image
void Detector::fetchFace()
{
	faces.clear();
	face_cascade.detectMultiScale(image, faces, HAAR_SCALE_FAC_PIC, HAAR_MIN_NEIGH_PIC, HAAR_FLAGS_PIC, cv::Size(HAAR_MIN_FACE_SIZE, HAAR_MIN_FACE_SIZE));
	for(int i=0; i<faces.size(); i++)
		rectangle(image, cv::Point(faces[i].x, faces[i].y), cv::Point(faces[i].x + faces[i].width, faces[i].y + faces[i].height), cv::Scalar(0));
}

//returns false if detector is good to go, otherwise returns true
bool Detector::isFaulty()
{
	return face_cascade.empty() || eye_cascade.empty();
}