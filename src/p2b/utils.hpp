#pragma once

#include "bitmap.hpp"
#include <opencv2/core/mat.hpp>
#include <string>


// ----------------------------------------------------------------------


namespace p2b {



void DEBUG_MSG(std::string msg);
void ERROR_MSG(std::string msg);
void PRINT_METRICS(cv::Mat img, p2b::Bitmap bitmap, double img2bmp_time, double bmp2img_time);



}   //? End of p2b namespace


