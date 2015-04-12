# FaceRecognition

FaceRecognition is an unfortunatelly picked name, since this program only detects 
(which is far from recognizing) face along with eyes and then normalizes it 
(puts eyeline horizontally and resizes it to 100x100)

usage: make && ./main <filepath>
or ./main -c for real-time webcam stream as a source (not implemented yet)

compiled with: make

It uses three main components: 
- Detector which localizes and cuts the face from the image and localizes eyes
- Normalizer which being given face and eyes positions' rotates, crops and 
resizes the image
- main used to communicate between the other two and manege the I/O

There is also a FaceData structure created which is basically a buffer with no methods
to make the communication between components simpler.

after printing the output pressing any key terminates the program

minimal size of a face for the program to find is set to min(75x75, imageWidth/10, imageHeight/10)