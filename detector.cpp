#include "detector.hpp"

//for sorting diminishingly
struct cmp
{
    bool operator()(cv::Rect a, cv::Rect b)
    {
        return a.area() > b.area();
    }
};

void Detector::init()
{
	if(!face_cascade.load(HAAR_FACE_PATH))
		throw std::runtime_error(STR_NO_FACE_CASC);
	if(!eye_cascade.load(HAAR_EYE_PATH))
		throw std::runtime_error(STR_NO_EYE_CASC);
}

//additional function allows us to change the arguments manually
void Detector::setArgumentsMan(float scaleFac_, int minNeigh_, int flags_)
{
	scaleFac = scaleFac_;
	minNeigh = minNeigh_;
	flags = flags_;
}

//sets the arguments for single-image search or real-time webcam search
void Detector::setArguments(int flag_)
{
	if(flag_ == ARGS_PIC)
		setArgumentsMan(HAAR_SCALE_FAC_PIC, HAAR_MIN_NEIGH_PIC, HAAR_FLAGS_PIC);
	if(flag_ == ARGS_CAM)
		setArgumentsMan(HAAR_SCALE_FAC_CAM, HAAR_MIN_NEIGH_CAM, HAAR_FLAGS_CAM);
}

//returns the presumed minimal size of a face to search for
inline cv::Size Detector::minFaceSize(int cols, int rows)
{
	return cv::Size(std::min(cols / HAAR_FACE_SEARCH_DIV, HAAR_MIN_FACE_SIZE),
					std::min(rows / HAAR_FACE_SEARCH_DIV, HAAR_MIN_FACE_SIZE));
}

//finds faces and eyes in the given image
FaceData Detector::fetchFaceAndEyes(cv::Mat image)
{
	std::vector <cv::Rect> faces, eyes, eyeCandidates;
	face_cascade.detectMultiScale(image, faces, scaleFac, minNeigh, flags, minFaceSize(image.cols, image.rows));

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
		eye_cascade.detectMultiScale(face, eyeCandidates, scaleFac, minNeigh, flags, 
			cv::Size(face.cols / HAAR_EYE_SEARCH_DIV, face.rows / HAAR_EYE_SEARCH_DIV));

		//now we should send to the normalizator a little more than the rectangle that contains face, because after
		//rotation (e.g. 45 degrees) we might lose a lot of the face in order for the image to remain rectangular
		//thus, we calculate how much more do we need and add it to our face-rectangle (however keeping old width
		//and height might be useful for rescaling the image in normalizer)
		int oldWidth = biggestFace.width, oldHeight = biggestFace.height;
		int diagonal = ceil(sqrt(biggestFace.width * biggestFace.width + biggestFace.height * biggestFace.height));
		cv::Point2f faceCenter = cv::Point2f(biggestFace.width / 2, biggestFace.height / 2); //center of the face
		int diffWidth, diffHeight; //how much have we moved the x and y of biggest face (will be needed for adjusting eyes)
		diffWidth = std::min((diagonal - biggestFace.width) / 2, biggestFace.x); //we cannot allow ourselves to have the coordinates negative...
		diffHeight = std::min((diagonal - biggestFace.height) / 2, biggestFace.y);
		faceCenter.x += diffWidth; //need to adjust the face center position...
		faceCenter.y += diffHeight;
		biggestFace.x -= diffWidth; //...as well as the face as a whole
		biggestFace.y -= diffHeight;
		biggestFace.width = std::min(diagonal, image.cols); //...or bigger than the size of the image
		biggestFace.height = std::min(diagonal, image.rows);

		//now we can get rid of the rest of the image since face is all we need
		image = image(cv::Rect(biggestFace.x, biggestFace.y, biggestFace.width, biggestFace.height)).clone();

		for(int i = 0; i < eyeCandidates.size(); i++)
		{
			//need to adjust the eyes' position to the bigger ROI
			eyeCandidates[i].x += diffWidth;
			eyeCandidates[i].y += diffHeight;

			//if we found an eye in the bottom part of the face - theyre probably not eyes
			if(eyeCandidates[i].y < image.rows / 2) //probably an actual eye
				eyes.push_back(eyeCandidates[i]);

			#ifdef DEBUG
				rectangle(image, cv::Point(eyeCandidates[i].x, eyeCandidates[i].y), 
						 cv::Point(eyeCandidates[i].x + eyeCandidates[i].width, eyeCandidates[i].y + eyeCandidates[i].height),
						 cv::Scalar(0));
			#endif
		}
		
		#ifdef DEBUG
			cv::namedWindow("dd", cv::WINDOW_NORMAL);
	    	cv::imshow("dd", image);
	   		cv::imwrite("temp.jpg", image);
    		cv::waitKey(0);
    		cv::destroyAllWindows();
    	#endif

    	std::sort(eyes.begin(), eyes.end(), cmp()); //sorting diminishingly

		if(eyes.size() > 1) //we need at least two eyes, take the two biggest ones
		{
			#ifdef DEBUG
				std::cout << eyes[0].area() << " " << eyes[1].area() << " " << eyes[2].area() << " " << eyes[eyes.size()-1].area() << "\n";
			#endif

			cv::Point2f eye1 = cv::Point2f((float) eyes[0].x + eyes[0].width / 2, (float) eyes[0].y + eyes[0].height / 2); //middle of the first eye
			cv::Point2f eye2 = cv::Point2f(0., 0.);

			//searchig for the other eye (should be on the other half of the face)
			for(int i = 1; i < eyes.size(); i++)
			{
				if((eyes[i].x - image.cols / 2) * (eyes[0].x - image.cols / 2) < 0) //checking if the eyes are on opposite sides
				{
					eye2 = cv::Point2f((float) eyes[i].x + eyes[i].width / 2, (float) eyes[i].y + eyes[i].height / 2); //middle of the second eye
					break;
				}
			}

			if(eye2.x == 0. && eye2.y == 0.) //second eye not found
				throw std::runtime_error(STR_EYES_NFOUND);

			#ifdef DEBUG
				std::cout << "picture size: " << image.cols << "x" << image.rows << "; eye positions: (" << eye1.x << ", " << eye1.y << "); (" << eye2.x << ", " << eye2.y <<
					"); old size: " << oldWidth << "x" << oldHeight << "; face center: (" << faceCenter.x << ", " << faceCenter.y << ")\n";
			#endif
			return FaceData(image, eye1, eye2, oldWidth, oldHeight, faceCenter);
		}
		else
			throw std::runtime_error(STR_EYES_NFOUND);

	}

	throw std::runtime_error(STR_FACE_NFOUND);
}

/*TODO: opens webcam window and finds the face on a real-time basis
void Detector::runCamera()
{
	cv::namedWindow("Camera_Output", 1);    //Create window
    //cv::Capture capture = cv::captureFromCAM(CV_CAP_ANY);
}
*/