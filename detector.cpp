#include "detector.hpp"

//for sorting diminishingly
struct cmp
{
    bool operator()(cv::Rect a, cv::Rect b)
    {
        return a.area() > b.area();
    }
};

//loading the cascades
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

//tries to finds by rotating the image by different angles and calling findFace on each
//NOTE: the bigger the angle the face leans by in the picture, the slower the program
//will work. That is caused by the order we rotate the face (first no rotation, then
//starting from the lowest angles both left and right) etc. depending on the ANGLE_NUM 
//constant we can check more or less of them it will work the slowest if there is no 
//face in the picture
FaceData Detector::fetchFace(cv::Mat image)
{
	int angles[] = {0, -35, 35, 70, 70}; //the more angles, the more drastically slow the program
	//appears to be, thus we only chose those angles, not the whole 360 - degrees circle
	FaceData data;
	for(int i = 0; i < sizeof(angles) / sizeof(angles[0]); i++)
	{
		cv::Mat trans = cv::getRotationMatrix2D(cv::Point2f(image.cols / 2, image.rows / 2), angles[i], 1.0);
		cv::Mat temp;
		cv::warpAffine(image, temp, trans, cv::Size(image.cols, image.rows));
		try
		{
			data = findFace(temp);
		}
        catch(std::exception& e)
		{
			//we do not want to stop looking after a single rotation found no face
			if(!strcmp(e.what(), STR_FACE_NFOUND))
				continue;
			throw std::runtime_error(e.what());
		}
		return data;
	}
	throw std::runtime_error(STR_FACE_NFOUND); 
}

//finds faces in the given image and chooses biggest one for eye search
FaceData Detector::findFace(cv::Mat image)
{
	std::vector <cv::Rect> faces;

	face_cascade.detectMultiScale(image, faces, scaleFac, minNeigh, flags, minFaceSize(image.cols, image.rows));

	//in case we found a face now we need to find the eyes
	if(!faces.empty())
	{
		//finding the biggest face found (highest chance of actually getting a face)
		cv::Rect biggestFace = faces[0];
		for(int i = 1; i < faces.size(); i++)
			if(faces[i].area() > biggestFace.area())
				biggestFace = faces[i];

		//saving the face found
		cv::Mat face = image(biggestFace);

		//now we should work on a little more than the rectangle that contains face, because after rotation
		//(e.g. 45 degrees) we might lose a lot of the face in order for the image to remain rectangular
		//thus we take twice the size (per side) image our face is (as if 2Wx2H instead of WxH),
		//however keeping old width and height might be useful for rescaling the image in normalizer
		int oldWidth = biggestFace.width, oldHeight = biggestFace.height;
		//int diagonal = ceil(sqrt(biggestFace.width * biggestFace.width + biggestFace.height * biggestFace.height));
		cv::Point2f faceCenter = cv::Point2f(biggestFace.width / 2, biggestFace.height / 2); //center of the face
		int diffWidth, diffHeight; //how much have we moved the x and y of biggest face (will be needed for adjusting eyes)
		diffWidth = std::min(oldWidth, biggestFace.x); //we cannot allow ourselves to have the coordinates negative...
		diffHeight = std::min(oldHeight, biggestFace.y);
		biggestFace.x -= diffWidth; //need to adjust the face position to bigger ROI
		biggestFace.y -= diffHeight;
		biggestFace.width = std::min(2 * oldWidth, image.cols - biggestFace.x); //...or bigger than the size of the image
		biggestFace.height = std::min(2 * oldHeight, image.rows - biggestFace.y);

		//now we can get rid of the rest of the image since face and a little more for rotation purposes is all we need
		image = image(cv::Rect(biggestFace.x, biggestFace.y, biggestFace.width, biggestFace.height));
		int angles[] = {0, -10, 10, -20, 20, -30, 30}; //rotation angles for eye searching in the given face
		int right_angle = 360; //what angle helped us find the eyes
		std::pair <cv::Point2f, cv::Point2f> eyes;
		//it is easier to find the eyes in verical faces, thus we try to rotate it by different angles
		for(int i = 0; i < sizeof(angles) / sizeof(angles[0]); i++)
		{
			cv::Mat trans = cv::getRotationMatrix2D(faceCenter, angles[i], 1.0);
			cv::Mat temp;
			cv::warpAffine(face, temp, trans, cv::Size(face.cols, face.rows));
			eyes = fetchEyes(temp);

	 		if(eyes == std::make_pair(cv::Point2f(0., 0.), cv::Point2f(0., 0.)))
	 			continue;
	 		else
	 		{
	 			right_angle = angles[i];
	 			break;
	 		}
		}

		if(right_angle == 360)
			throw std::runtime_error(STR_EYES_NFOUND);

		faceCenter.x += diffWidth; //need to adjust the face center position to the bigger ROI
		faceCenter.y += diffHeight;
		eyes.first.x += diffWidth; //need to adjust the eyes' position to the bigger ROI
		eyes.first.y += diffHeight;
		eyes.second.x += diffWidth;
		eyes.second.y += diffHeight;
		

    	if(right_angle) //rotating the bigger ROI by the right angle
		{
			cv::Mat trans = cv::getRotationMatrix2D(faceCenter, right_angle, 1.0);
			cv::warpAffine(image, image, trans, cv::Size(image.cols, image.rows));
		}

		return FaceData(image, eyes.first, eyes.second, oldWidth, oldHeight, faceCenter);
	}
	else
		throw std::runtime_error(STR_FACE_NFOUND);
}

//function finds eyes in a given face
std::pair <cv::Point2f, cv::Point2f> Detector::fetchEyes(cv::Mat face)
{
		std::vector <cv::Rect> eyes[2], eyeCandidates[2]; //for left and right eyes
		//searching for eyes
		cv::Rect searchArea = cv::Rect(0, 0, face.cols / 2, face.rows);
		eye_cascade.detectMultiScale(face(searchArea), eyeCandidates[LEFT], scaleFac, minNeigh, flags, 
			cv::Size(face.cols / HAAR_EYE_SEARCH_DIV, face.rows / HAAR_EYE_SEARCH_DIV));
		searchArea = cv::Rect(face.cols / 2, 0, face.cols / 2, face.rows);
		eye_cascade.detectMultiScale(face(searchArea), eyeCandidates[RIGHT], scaleFac, minNeigh, flags, 
			cv::Size(face.cols / HAAR_EYE_SEARCH_DIV, face.rows / HAAR_EYE_SEARCH_DIV));
		for(int j = 0; j < 2; j++) //for left and right eyes
		{ 
			for(int i = 0; i < eyeCandidates[j].size(); i++)
			{
				if(j == RIGHT) //need to adjust the rectangle to the whole face (not half of it)
					eyeCandidates[j][i].x += face.cols / 2;
				//we reject the candidate if the eye is:
				if((double) eyeCandidates[j][i].area() / (face.cols * face.rows) > MAX_EYE_FACE_RATIO || //too big
					(double) eyeCandidates[j][i].area() / (face.cols * face.rows) < MIN_EYE_FACE_RATIO || //too small
					eyeCandidates[j][i].y + eyeCandidates[j][i].height / 2 > face.rows / 2) //in the bottom part of the face
					continue;

				eyes[j].push_back(eyeCandidates[j][i]);

				#ifdef DEBUG
					printf("ratio: %f, ", (double) eyeCandidates[j][i].area() / (face.cols * face.rows));
					rectangle(face, cv::Point(eyeCandidates[j][i].x, eyeCandidates[j][i].y), 
							 cv::Point(eyeCandidates[j][i].x + eyeCandidates[j][i].width, eyeCandidates[j][i].y + eyeCandidates[j][i].height),
							 cv::Scalar(0));
				#endif
			}
		}
		
		#ifdef DEBUG
			cv::namedWindow("dd", cv::WINDOW_NORMAL);
	    	cv::imshow("dd", face);
	   		cv::imwrite("temp.jpg", face);
    		cv::waitKey(0);
    		cv::destroyAllWindows();
    	#endif

    	std::sort(eyes[LEFT].begin(), eyes[LEFT].end(), cmp()); //sorting diminishingly
    	std::sort(eyes[RIGHT].begin(), eyes[RIGHT].end(), cmp());

		if(eyes[LEFT].size() && eyes[RIGHT].size()) //we need at least one left and right eye
		{
			 //finding the middle points of the eyes
			cv::Point2f leye = cv::Point2f((float) eyes[LEFT][0].x + eyes[LEFT][0].width / 2, (float) eyes[LEFT][0].y + eyes[LEFT][0].height / 2);
			cv::Point2f reye = cv::Point2f((float) eyes[RIGHT][0].x + eyes[RIGHT][0].width / 2, (float) eyes[RIGHT][0].y + eyes[RIGHT][0].height / 2);
			#ifdef DEBUG
				std::cout << "picture size: " << face.cols << "x" << face.rows << "; eye positions: (" 
					<< leye.x << ", " << leye.y << "); (" << reye.x << ", " << reye.y << ");\n";
			#endif
			return std::make_pair(leye, reye);
		}
		else
			return std::make_pair(cv::Point2f(0., 0.), cv::Point2f(0., 0.));
}
