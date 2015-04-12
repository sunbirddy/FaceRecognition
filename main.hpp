#ifndef MAIN_HPP
#define MAIN_HPP

#include <stdexcept>
#include <iostream>
#include <string>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"


#define STR_USAGE_INSTRUCTION "Usage: ./main <filename> or ./main -c (for webcam)\n"
#define MAX_WAIT_TIME 60000
#define SAVING_PATH "face.jpg"

void showAndSaveImage(cv::Mat image);
cv::Mat getImage(std::string path);

#endif