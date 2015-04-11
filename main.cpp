#include "main.hpp"
#include "detector.hpp"
#include "normalizator.hpp"

int main(int argc, char ** argv)
{
	Detector det = Detector();

	if(det.isFaulty())
		return 0;

	if(!det.getImage())
	{
		puts(STR_READ_FAILURE);
		return -1;
	}
	
	while(true)
	{
		det.fetchFace();
		det.showImage();
	}

	return 0;
}