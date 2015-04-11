CC=g++
CFRONT=-O2
CBACK=-lopencv_core -lopencv_highgui -lopencv_contrib -lopencv_imgproc -lopencv_objdetect

All:
	$(CC) $(CFRONT) main.cpp detector.cpp normalizator.cpp $(CBACK) -o main