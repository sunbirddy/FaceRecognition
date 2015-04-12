# FaceRecognition

FaceRecognition is an unfortunatelly picked name, since this program only detects 
(which is far from recognizing) face along with eyes and then normalizes it 
(puts eyeline horizontally and resizes it to 100x100)

usage: make && ./main <filepath>

after printing the output pressing any key terminates the program

minimal size of a face for the program to find is set to 10x10

compiled with: make

in order for compiling to success, one needs to have haar cascades installed and 
moved into directories:
/usr/share/opencv/haarcascades/haarcascade_frontalface_alt.xml
/usr/share/opencv/haarcascades/haarcascade_eye.xml