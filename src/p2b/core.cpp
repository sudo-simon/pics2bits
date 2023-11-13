#include "core.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>


using namespace std;


// ----------------------------------------------------------------------





p2b::Bitmap p2b::toBitmap(cv::Mat img, uint8_t pixel_size, const std::vector<uint8_t>& thresholds_v, bool parallel){
    
    uint8_t pixels_per_byte = 8/pixel_size;

    size_t bm_rows = img.rows;
    size_t bm_cols = img.cols;
    while ((bm_cols % pixels_per_byte) != 0){
        ++bm_cols;
    }
    bm_cols /= pixels_per_byte;

    p2b::Bitmap ret_bm = Bitmap(bm_rows, bm_cols, pixel_size, thresholds_v);
    
    if (parallel) ret_bm.fromImage_parallel(img);
    else ret_bm.fromImage_linear(img);

    return ret_bm;

}








vector<vector<uint8_t>> p2b::toBits(cv::Mat img, uint8_t pixel_size, const std::vector<uint8_t>& thresholds_v, bool parallel){
    
    uint8_t pixels_per_byte = 8/pixel_size;

    size_t bm_rows = img.rows;
    size_t bm_cols = img.cols;
    while ((bm_cols % pixels_per_byte) != 0){
        ++bm_cols;
    }
    bm_cols /= pixels_per_byte;

    p2b::Bitmap bm = Bitmap(bm_rows, bm_cols, pixel_size, thresholds_v);
    
    if (parallel) bm.fromImage_parallel(img);
    else bm.fromImage_linear(img);
    
    return bm.getVec();

}


/*
int p2b::addBits(Bitmap* bitmap_p, cv::Mat add_img, int add_direction){
    //TODO
}



int p2b::updateBitmap(Bitmap* bitmap_p, cv::Mat updated_img){
    //TODO
}
*/



