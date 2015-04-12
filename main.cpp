#include "main.hpp"
#include "detector.hpp"

int main(int argc, const char * argv[])
{
	Detector det;
	try 
	{
		det = Detector();
	}
	catch(std::exception& e)
	{
		std::cerr << e.what();
		return -1;
	}

	//Normalizator norm = Normalizator();
	std::string webcamFlag = "-c";

	if(det.isFaulty()) //checking if we managed to load the cascades
		return 0;
	
	if(argc < 2) //too little arguments
	{
		std::cerr << STR_USAGE_INSTRUCTION;
		return -1;
	}

	std::string argument = argv[1];

	if(argument == webcamFlag) //running camera window
	{
		det.runCamera();
		return 0;
	}


	//processing a single image
	if(!det.getImage(argument))
		return -1;

	if(!det.fetchFace())
		return -1;

	return 0;
}