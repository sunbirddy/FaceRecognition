#include "detector.hpp"

//for sorting diminishingly
struct cmp
{
    bool operator()(cv::Rect a, cv::Rect b)
    {
        return a.area() > b.area();
    }
};

Detector::Detector()
{
	if(!face_cascade.load(HAAR_FACE_PATH))
		throw std::runtime_error(STR_NO_FACE_CASC);
	if(!eye_cascade.load(HAAR_EYE_PATH))
		throw std::runtime_error(STR_NO_EYE_CASC);
}

//returns the presumed minimal size of a face to search for
inline cv::Size Detector::minFaceSize(int cols, int rows)
{
	return cv::Size(std::min(cols / HAAR_FACE_SEARCH_DIV, HAAR_MIN_FACE_SIZE),
					std::min(rows / HAAR_FACE_SEARCH_DIV, HAAR_MIN_FACE_SIZE));
}

//finds faces and eyes in the given image
FaceData Detector::fetchFace(cv::Mat image)
{
	std::vector <cv::Rect> faces, eyes;
	face_cascade.detectMultiScale(image, faces, HAAR_SCALE_FAC_PIC, 
		HAAR_MIN_NEIGH_PIC, HAAR_FLAGS_PIC, minFaceSize(image.cols, image.rows));

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
			HAAR_MIN_NEIGH_PIC, HAAR_FLAGS_PIC, cv::Size(face.cols / HAAR_EYE_SEARCH_DIV, face.rows / HAAR_EYE_SEARCH_DIV));

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
			//if we found an eye in the bottom part of the face - theyre probably not eyes
			if(eyes[i].y >= image.rows / 2)
			{
				//removing the eye
				std::swap(eyes[i], eyes[eyes.size() - 1]);
				eyes.pop_back();
				i--;
			}

			//need to adjust the eyes' position to the bigger ROI
			eyes[i].x += diffWidth;
			eyes[i].y += diffHeight;
			#ifdef DEBUG
				rectangle(image, cv::Point(eyes[i].x, eyes[i].y), cv::Point(eyes[i].x + eyes[i].width, eyes[i].y + eyes[i].height), cv::Scalar(0));
			#endif
		}
		
		#ifdef DEBUG
			cv::namedWindow("dd", cv::WINDOW_NORMAL);
	    	cv::imshow("dd", image);
	   		cv::imwrite("face.jpg", image);
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

			return FaceData(image, eye1, eye2, oldWidth, oldHeight);
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