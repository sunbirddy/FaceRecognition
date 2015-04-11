#include "main.hpp"
#include "detector.hpp"

int main(int argc, char ** argv)
{
	Detector det = Detector();

	if(det.isFaulty()) //checking if we managed to load the cascades
		return 0;
	
	while(true)
	{
		if(!det.getImage())
		{
			puts(STR_READ_FAILURE);
			return -1;
		}
		det.fetchFace();
	}

	return 0;
}