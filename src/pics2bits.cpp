#include "include/pics2bits.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <vector>



using namespace p2b;
using namespace std;

Bitmap::Bitmap(size_t rows, size_t cols, uint8_t pixel_size, vector<uint8_t> thresholds_v){
    if (rows <= 0 || cols <= 0){
        ERROR_MSG("rows and cols are not both greater than zero");
        exit(1);
    }
    if ((pixel_size < 1) || (pixel_size > 4) || (pixel_size & (pixel_size-1)) != 0){
        ERROR_MSG("pixel_size is not one of {1, 2, 4}");
        exit(1);
    }
    if (thresholds_v.size() != ((size_t) pow(2,pixel_size) - 1)){
        ERROR_MSG("thresholds_v is not a vector of 8/pixel_size thresholds to apply");
        exit(1);
    }
    if (! is_sorted(thresholds_v.begin(), thresholds_v.end())){
        ERROR_MSG("thresholds_v is not sorted in ascending order");
        exit(1);
    }
    for (uint8_t& threshold : thresholds_v){
        if (threshold < 0 || threshold > 255){
            ERROR_MSG("every threshold in thresholds_v must be between 0 and 255");
            exit(1);
        }
    }
    
    this->rows = rows;
    this->cols = cols;
    this->pixel_size = pixel_size;
    this->thresholds_v = thresholds_v;
    this->vec = vector<vector<uint8_t>>(rows, vector<uint8_t>(cols,255));
}


size_t Bitmap::getRows(){ return this->rows; }
size_t Bitmap::getCols(){ return this->cols; }


int Bitmap::increaseSize(size_t new_rows, size_t new_cols){
    if ((new_rows < this->rows) || (new_cols < this->cols)){
        ERROR_MSG("new_rows and new_cols must be greater than the existing rows and cols");
        return 1;
    }

    this->vec.reserve(new_rows);
    for (vector<uint8_t>& row_v : this->vec){
        row_v.reserve(new_cols);
    }
    return 0;
}


int Bitmap::doubleSize(){
    this->vec.reserve(this->rows*2);
    for (vector<uint8_t>& row_v : this->vec){
        row_v.reserve(this->cols*2);
    }
    return 0;
}


int Bitmap::fromImage(cv::Mat img){
    int img_rows = img.rows;
    int img_cols = img.cols;
    cv::Mat gs_img = img;
    if (img.channels() > 1){
        cv::cvtColor(img, gs_img, cv::COLOR_BGR2GRAY);
    }

    uint8_t pixels_per_byte = 8/(this->pixel_size);
    size_t bitmap_j;
    uint8_t p_value;
    uint8_t l_shift;
    
    for (size_t i=0; i<img_rows; ++i){
        for (size_t j=0; j<img_cols; ++j){

            bitmap_j = j/pixels_per_byte;
            
            p_value = 0;
            for (uint8_t& threshold : this->thresholds_v){
                if (gs_img.at<uint8_t>(i,j) < threshold) break;
                ++p_value;
            }

            l_shift = (8-pixel_size)-((j%pixels_per_byte)*pixel_size); //TODO

        }
    }
}






