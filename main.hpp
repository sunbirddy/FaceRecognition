#ifndef MAIN_HPP
#define MAIN_HPP

//#define DEBUG
#include "facedata.hpp"
#include "detector.hpp"
#include "normalizator.hpp"
#include <stdexcept>
#include <iostream>
#include <string>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#define FLAG_CAM "-c"
#define STR_USAGE_INSTRUCTION "Usage: ./main <filename> or ./main -c (for webcam)\n"
#define STR_READ_FAILURE "Could not read the image\n"
#define STR_CAM_CLOSED "Could not open the camera\n"
#define STR_CAM_READ_FAILURE "Could not read frame from the camera\n"
#define STR_CAM_WINDOW_TITLE "Webcam capture"
#define MAX_WAIT_TIME 60000
#define MAX_WAIT_TIME_CAM 30
#define ESC_KEY 27
#define SAVING_PATH "face.jpg"

void showAndSaveImage(cv::Mat image);
cv::Mat getImage(std::string path);
void runCamera(Detector * det, Normalizator * norm);

#endif