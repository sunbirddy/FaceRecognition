#include "main.hpp"
#include "facedata.hpp"
#include "detector.hpp"
#include "normalizator.hpp"

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
//TODO: for path = "-c" it reads image from webcam
cv::Mat getImage(std::string path)
{
	cv::Mat image = cv::imread(path, CV_LOAD_IMAGE_GRAYSCALE);
	if(!image.data)
		throw std::runtime_error(STR_READ_FAILURE);
	return image;
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

	Normalizator norm = Normalizator();
	FaceData data; //buffer used to send data from detector to normalizator

	std::string webcamFlag = "-c";

	if(argc < 2) //too little arguments
	{
		std::cerr << STR_USAGE_INSTRUCTION;
		return -1;
	}

	std::string argument = argv[1];

	if(argument == webcamFlag) //running camera window
	{
		//TODO:
		//det.runCamera();
		return 0;
	}

	//processing a single image
	try
	{
		data.image = getImage(argument);
		data = det.fetchFaceAndEyes(data.image);
		showAndSaveImage(norm.normalize(data));
	}
	catch(std::exception& e)
	{
		std::cerr << e.what();
		return -1;
	}

	return 0;
}