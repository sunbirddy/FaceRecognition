#include "main.hpp"

#ifdef DEBUG
	std::string positiveIntToStr(int n)
	{
     	std::string tmp, ret;
     	if(n < 0) 
     	{
      		ret = "-";
      		n = -n;
     	}
     	do{
      		tmp += n % 10 + 48;
      		n -= n % 10;
     	}
     	while(n /= 10);
     	for(int i = tmp.size()-1; i >= 0; i--)
      		ret += tmp[i];
     	return ret;
	}
#endif

//shows image in a new window
void showAndSaveImage(cv::Mat image)
{
	cv::imwrite(SAVING_PATH, image);
	cv::namedWindow(STR_NORMALIZATION_SUCCESS, cv::WINDOW_AUTOSIZE);
    cv::imshow(STR_NORMALIZATION_SUCCESS, image);
    cv::waitKey(MAX_WAIT_TIME);
    cv::destroyAllWindows();
}

//reads image from file
//for path = "-c" it reads image from webcam
cv::Mat getImage(std::string path)
{
	cv::Mat image = cv::imread(path, CV_LOAD_IMAGE_GRAYSCALE);
	if(!image.data)
		throw std::runtime_error(STR_READ_FAILURE);
	return image;
}

//opens webcam window and enables real-time face and eyes search + normalization
//in another window
void runCamera(Detector * det, Normalizator * norm)
{
	cv::namedWindow(STR_CAM_WINDOW_TITLE, cv::WINDOW_NORMAL); //webcam window
	cv::namedWindow(STR_NORMALIZATION_SUCCESS, cv::WINDOW_AUTOSIZE); //output window
	cv::VideoCapture capture(CV_CAP_ANY);
	if(!capture.isOpened())
		throw std::runtime_error(STR_CAM_CLOSED);
	cv::Mat frame;
	int i = 0;
	while(++i)
	{
		if(!capture.read(frame))
			throw std::runtime_error(STR_CAM_READ_FAILURE);
		cv::imshow(STR_CAM_WINDOW_TITLE, frame);
		std::cout << "frames processed: " << i << "; press any key to exit\n";
		if(cv::waitKey(MAX_WAIT_TIME_CAM) != -1) //wait for ESCAPE key press for a short time period
			return;
		cv::cvtColor(frame, frame, CV_RGB2GRAY); //conversion to greyscale
		#ifdef DEBUG //storing the frames and faces for debugging purposes
			cv::imwrite("cache/frames/" + std::to_string(i) + ".jpg", frame);
        #endif
        try //outputs the normalized face
        {
			frame = norm->normalize(det->fetchFace(frame));
        	#ifdef DEBUG
        		cv::imwrite("cache/faces/" + std::to_string(i) + ".jpg", frame);
        	#endif
        	cv::imshow(STR_NORMALIZATION_SUCCESS, frame);
        }
		catch(std::exception& e)
		{
			std::cerr << e.what();
			//we do not want our program to stop after a single frame it has not found face in, do we?
			if(!strcmp(e.what(), STR_FACE_NFOUND) || !strcmp(e.what(), STR_EYES_NFOUND))
				continue;
			return;
		}
	}
}

int main(int argc, const char * argv[])
{
	Detector det;
	//DO NOT put any code in here!
	//the detector must be properly initialized before performing any actions on it
	try 
	{
		det.init();
	}
	catch(std::exception& e)
	{
		std::cerr << e.what();
		return -1;
	}

	#ifdef DEBUG
		//det.setArguments(ARGS_CAM);
	#endif
	
	Normalizator norm = Normalizator();
	FaceData data; //buffer used to send data from detector to normalizator

	std::string webcamFlag = FLAG_CAM;

	if(argc < 2) //too little arguments
	{
		std::cerr << STR_USAGE_INSTRUCTION;
		return -1;
	}

	std::string argument = argv[1];

	if(argument == webcamFlag) //running camera window
	{
		det.setArguments(ARGS_CAM); //adjusting settings for webcam capture search
		try
		{
			runCamera(&det, &norm);
		}
		catch(std::exception& e)
		{
			std::cerr << e.what();
			return -1;
		}
		return 0;
	}

	//processing a single image
	try
	{
		data.image = getImage(argument);
		data = det.fetchFace(data.image);
		showAndSaveImage(norm.normalize(data));
	}
	catch(std::exception& e)
	{
		std::cerr << e.what();
		return -1;
	}

	return 0;
}