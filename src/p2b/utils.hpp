#pragma once

#include "bitmap.hpp"
#include <opencv2/core/mat.hpp>
#include <string>


// ----------------------------------------------------------------------


namespace p2b {



void DEBUG_MSG(std::string msg);
void ERROR_MSG(std::string msg);
void PRINT_METRICS(cv::Mat img, p2b::Bitmap bitmap, long img2bmp_time_ms, long bmp2img_time_ms);



}   //? End of p2b namespace


