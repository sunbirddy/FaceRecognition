#include "main.hpp"
#include "detector.hpp"

int main(int argc, char * argv[])
{
	Detector det = Detector();
	
	if(det.isFaulty()) //checking if we managed to load the cascades
		return 0;
	
	if(argc < 2)
	{
		puts(STR_USAGE_INSTRUCTION);
		return 0;
	}

	if(!det.getImage(argv[1]))
		return -1;

	if(!det.fetchFace())
		return -1;

	return 0;
}