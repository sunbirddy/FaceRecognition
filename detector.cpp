#include "detector.hpp"

Detector::Detector()
{
	if(!face_cascade.load(HAAR_FACE_PATH))
		throw std::runtime_error(STR_NO_FACE_CASC);
	if(!eye_cascade.load(HAAR_EYE_PATH))
		throw std::runtime_error(STR_NO_EYE_CASC);
}

//reads image from file
//TODO: add reading images from webcam
bool Detector::getImage(std::string path)
{
	image = cv::imread(path, CV_LOAD_IMAGE_GRAYSCALE);
	if(!image.data)
	{
		std::cerr << STR_READ_FAILURE;
		return false;
	}
	return true;
}

//finds faces and eyes in the given image
bool Detector::fetchFace()
{
	std::vector <cv::Rect> faces, eyes;
	face_cascade.detectMultiScale(image, faces, HAAR_SCALE_FAC_PIC, 
		HAAR_MIN_NEIGH_PIC, HAAR_FLAGS_PIC, cv::Size(HAAR_MIN_FACE_SIZE, HAAR_MIN_FACE_SIZE));

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
		//thus, we calculate how much more do we need and add it to our face-rectangle (however keeping old width
		//and height might be useful for rescaling the image in normalizer)
		int oldWidth = biggestFace.width, oldHeight = biggestFace.height;
		int diagonal = ceil(sqrt(biggestFace.width * biggestFace.width + biggestFace.height * biggestFace.height));
		int diffWidth, diffHeight; //how much have we moved the x and y of biggest face (will be needed for adjusting eyes)
		diffWidth = (diagonal - biggestFace.width) / 2;
		diffHeight = (diagonal - biggestFace.height) / 2;
		biggestFace.x = std::max(0, biggestFace.x - diffWidth); //we cannot allow ourselves to have the coordinates negative...
		biggestFace.y = std::max(0, biggestFace.y - diffHeight);
		biggestFace.width = std::min(diagonal, image.cols); //...or bigger than the size of the image
		biggestFace.height = std::min(diagonal, image.rows);

		//now we can get rid of the rest of the image since face is all we need
		image = image(cv::Rect(biggestFace.x, biggestFace.y, biggestFace.width, biggestFace.height)).clone();

		for(int i = 0; i < eyes.size(); i++)
		{
			//need to adjust the eyes' position to the bigger ROI
			eyes[i].x += diffWidth;
			eyes[i].y += diffHeight;
		}

		if(eyes.size() > 1) //we need at least two eyes
		{
			cv::Point2f leye = cv::Point2f((float) eyes[0].x + eyes[0].width / 2, (float) eyes[0].y + eyes[0].height / 2); //middle of the left...
			cv::Point2f reye = cv::Point2f((float) eyes[1].x + eyes[1].width / 2, (float) eyes[1].y + eyes[1].height / 2); //...and the right eye
			
			Normalizator norm = Normalizator(image, leye, reye, oldWidth, oldHeight);
			norm.normalize();
			norm.showNormalizedImage();
		}
		else
		{	
			std::cerr << STR_EYES_NFOUND;
			return false;
		}

		return true;
	}

	std::cerr << STR_FACE_NFOUND;
	return false;	
}

//opens webcam window and finds the face on a real-time basis
void Detector::runCamera()
{
	cv::namedWindow("Camera_Output", 1);    //Create window
    //cv::Capture capture = cv::captureFromCAM(CV_CAP_ANY);
}

//returns false if detector is good to go, otherwise returns true
bool Detector::isFaulty()
{
	return face_cascade.empty() || eye_cascade.empty();
}