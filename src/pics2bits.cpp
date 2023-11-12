#include "include/pics2bits.hpp"

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



using namespace p2b;
using namespace std;



Bitmap::Bitmap(){
    this->rows = 0;
    this->cols = 0;
    this->pixel_size = 1;
    this->pixels_per_byte = 8;
    this->pixel_values = 1;
    this->thresholds_v = {125};
    this->vec = vector<vector<uint8_t>>(0,vector<uint8_t>(0));
}



Bitmap::Bitmap(size_t rows, size_t cols, uint8_t pixel_size, const vector<uint8_t>& thresholds_v){
    
    if (rows <= 0 || cols <= 0){
        ERROR_MSG("rows and cols are not both greater than zero");
        exit(1);
    }
    if ((pixel_size < 1) || (pixel_size > 4) || (pixel_size & (pixel_size-1)) != 0){    //! Maybe I could just check the 3 possible values...
        ERROR_MSG("pixel_size is not one of {1, 2, 4}");
        exit(1);
    }
    if (thresholds_v.size() != ((size_t) (1 << pixel_size) - 1)){
        ERROR_MSG("thresholds_v is not a vector of 8/pixel_size thresholds to apply");
        exit(1);
    }
    if (! is_sorted(thresholds_v.begin(), thresholds_v.end())){
        ERROR_MSG("thresholds_v is not sorted in ascending order");
        exit(1);
    }
    for (const uint8_t& threshold : thresholds_v){
        if (threshold < 0 || threshold > 255){
            ERROR_MSG("every threshold in thresholds_v must be between 0 and 255");
            exit(1);
        }
    }
    
    this->rows = rows;
    this->cols = cols;
    this->pixel_size = pixel_size;
    this->pixels_per_byte = 8/pixel_size;
    this->pixel_values = (1 << pixel_size) -1;
    this->thresholds_v = thresholds_v;

    this->vec = vector<vector<uint8_t>>(rows, vector<uint8_t>(cols,255));

}



size_t Bitmap::getRows(){ return this->rows; }
size_t Bitmap::getCols(){ return this->cols; }
vector<vector<uint8_t>> Bitmap::getVec(){ return this->vec; }



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
    
    size_t img_rows = img.rows;
    size_t img_cols = img.cols;
    cv::Mat gs_img = img;
    if (img.channels() > 1){
        cv::cvtColor(img, gs_img, cv::COLOR_BGR2GRAY);
    }

    size_t vec_j;
    uint8_t p_value;
    uint8_t l_shift;
    
    for (size_t i=0; i<img_rows; ++i){
        for (size_t j=0; j<img_cols; ++j){

            vec_j = j/this->pixels_per_byte;
            
            /* 
                ? Because of how this piece of code works, if we want to keep the 1, 11, 1111 values reserved
                ? we have to ensure that the last threshold value is 255 for pixel_size != 1
            */
            p_value = 0;
            for (uint8_t& threshold : this->thresholds_v){
                if (gs_img.at<uint8_t>(i,j) < threshold)
                    break;
                ++p_value;
            }

            l_shift = (8-this->pixel_size) - ((j%this->pixels_per_byte) * this->pixel_size); //TODO check this formula

            /*
                ? This algorithm that performs bitwise operations only works
                ? if the pixels of teh bitmap we're accessing are initialized
                ? in sequential order, like in this case, since the biwise OR
                ? operations together with the &= could alter previously initialized
                ? pixels if those are not 1, 11, 1111
            */

            switch (this->pixel_size) {
                
                case 1:
                    switch (l_shift) {
                        case 7:
                            this->vec[i][vec_j] &= ((p_value << l_shift) | OR_MASK_P1_0);
                            break;
                        case 6:
                            this->vec[i][vec_j] &= ((p_value << l_shift) | OR_MASK_P1_1);
                            break;
                        case 5:
                            this->vec[i][vec_j] &= ((p_value << l_shift) | OR_MASK_P1_2);
                            break;
                        case 4:
                            this->vec[i][vec_j] &= ((p_value << l_shift) | OR_MASK_P1_3);
                            break;
                        case 3:
                            this->vec[i][vec_j] &= ((p_value << l_shift) | OR_MASK_P1_4);
                            break;
                        case 2:
                            this->vec[i][vec_j] &= ((p_value << l_shift) | OR_MASK_P1_5);
                            break;
                        case 1:
                            this->vec[i][vec_j] &= ((p_value << l_shift) | OR_MASK_P1_6);
                            break;
                        case 0:
                            this->vec[i][vec_j] &= ((p_value << l_shift) | OR_MASK_P1_7);
                            break;
                        default:
                            ERROR_MSG("unexpected l_shift value in fromImage");
                            exit(1);
                            break;
                    }
                    break;

                case 2:
                    switch (l_shift) {
                        case 6:
                            this->vec[i][vec_j] &= ((p_value << l_shift) | OR_MASK_P2_0);
                            break;
                        case 4:
                            this->vec[i][vec_j] &= ((p_value << l_shift) | OR_MASK_P2_1);
                            break;
                        case 2:
                            this->vec[i][vec_j] &= ((p_value << l_shift) | OR_MASK_P2_2);
                            break;
                        case 0:
                            this->vec[i][vec_j] &= ((p_value << l_shift) | OR_MASK_P2_3);
                            break;
                        default:
                            ERROR_MSG("unexpected l_shift value in fromImage");
                            exit(1);
                            break;
                    }
                    break;

                case 4:
                    switch (l_shift) {
                        case 4:
                            this->vec[i][vec_j] &= ((p_value << l_shift) | OR_MASK_P4_0);
                            break;
                        case 0:
                            this->vec[i][vec_j] &= ((p_value << l_shift) | OR_MASK_P4_1);
                            break;
                        default:
                            ERROR_MSG("unexpected l_shift value in fromImage");
                            exit(1);
                            break;
                    }
                    break;
                
                default:
                    ERROR_MSG("invalid pixel_size value in fromImage");
                    exit(1);
                    break;

            }

        }
    }

    return 0;

}



int Bitmap::toGrayscaleImage(cv::Mat* dst_img, const vector<uint8_t>& grayscale_palette){
    
    if (grayscale_palette.size() != this->pixel_values){
        ERROR_MSG("grayscale_palette size doesn't match pixel_values");
        return 1;
    }

    size_t img_rows = this->rows;
    size_t img_cols = this->cols * this->pixels_per_byte;
    dst_img->create(img_rows,img_cols,CV_8UC1);

    size_t vec_j;
    uint8_t p_value;
    uint8_t r_shift;

    for (size_t i=0; i<img_rows; ++i){
        for (size_t j=0; j<img_cols; ++j){

            vec_j = j/this->pixels_per_byte;
            r_shift = (8-this->pixel_size) - ((j%this->pixels_per_byte) * this->pixel_size);

            switch (this->pixel_size) {
            
                case 1:
                    switch (r_shift) {
                        case 7:
                            p_value = (this->vec[i][vec_j] & AND_MASK_P1_0) >> r_shift;
                            break;
                        case 6:
                            p_value = (this->vec[i][vec_j] & AND_MASK_P1_1) >> r_shift;
                            break;
                        case 5:
                            p_value = (this->vec[i][vec_j] & AND_MASK_P1_2) >> r_shift;
                            break;
                        case 4:
                            p_value = (this->vec[i][vec_j] & AND_MASK_P1_3) >> r_shift;
                            break;
                        case 3:
                            p_value = (this->vec[i][vec_j] & AND_MASK_P1_4) >> r_shift;
                            break;
                        case 2:
                            p_value = (this->vec[i][vec_j] & AND_MASK_P1_5) >> r_shift;
                            break;
                        case 1:
                            p_value = (this->vec[i][vec_j] & AND_MASK_P1_6) >> r_shift;
                            break;
                        case 0:
                            p_value = (this->vec[i][vec_j] & AND_MASK_P1_7) >> r_shift;
                            break;
                        default:
                            ERROR_MSG("unexpected r_shift value in toGrayscaleImage");
                            exit(1);
                            break;
                    }
                    break;

                case 2:
                    switch (r_shift) {
                        case 6:
                            p_value = (this->vec[i][vec_j] & AND_MASK_P2_0) >> r_shift;
                            break;
                        case 4:
                            p_value = (this->vec[i][vec_j] & AND_MASK_P2_1) >> r_shift;
                            break;
                        case 2:
                            p_value = (this->vec[i][vec_j] & AND_MASK_P2_2) >> r_shift;
                            break;
                        case 0:
                            p_value = (this->vec[i][vec_j] & AND_MASK_P2_3) >> r_shift;
                            break;
                        default:
                            ERROR_MSG("unexpected r_shift value in toGrayscaleImage");
                            exit(1);
                            break;
                    }
                    break;

                case 4:
                    switch (r_shift) {
                        case 4:
                            p_value = (this->vec[i][vec_j] & AND_MASK_P4_0) >> r_shift;
                            break;
                        case 0:
                            p_value = (this->vec[i][vec_j] & AND_MASK_P4_1) >> r_shift;
                            break;
                        default:
                            ERROR_MSG("unexpected r_shift value in toGrayscaleImage");
                            exit(1);
                            break;
                    }
                    break;
                
                default:
                    ERROR_MSG("invalid pixel_size value in toGrayscaleImage");
                    exit(1);
                    break;

            }

            dst_img->at<uint8_t>(i,j) = (p_value!=this->pixel_values) ? grayscale_palette[p_value] : 0;

        }
    }

    return 0;

}



int Bitmap::toBGRImage(cv::Mat* dst_img, const vector<cv::Vec3b>& BGR_palette){
    
    if (BGR_palette.size() != this->pixel_values){
        ERROR_MSG("BGR_palette size doesn't match pixel_values");
        return 1;
    }

    size_t img_rows = this->rows;
    size_t img_cols = this->cols * this->pixels_per_byte;
    dst_img->create(img_rows,img_cols,CV_8SC3);

    size_t vec_j;
    uint8_t p_value;
    uint8_t r_shift;

    for (size_t i=0; i<img_rows; ++i){
        for (size_t j=0; j<img_cols; ++j){

            vec_j = j/this->pixels_per_byte;
            r_shift = (8-this->pixel_size) - ((j%this->pixels_per_byte) * this->pixel_size);

            switch (this->pixel_size) {
            
                case 1:
                    switch (r_shift) {
                        case 7:
                            p_value = (this->vec[i][vec_j] & AND_MASK_P1_0) >> r_shift;
                            break;
                        case 6:
                            p_value = (this->vec[i][vec_j] & AND_MASK_P1_1) >> r_shift;
                            break;
                        case 5:
                            p_value = (this->vec[i][vec_j] & AND_MASK_P1_2) >> r_shift;
                            break;
                        case 4:
                            p_value = (this->vec[i][vec_j] & AND_MASK_P1_3) >> r_shift;
                            break;
                        case 3:
                            p_value = (this->vec[i][vec_j] & AND_MASK_P1_4) >> r_shift;
                            break;
                        case 2:
                            p_value = (this->vec[i][vec_j] & AND_MASK_P1_5) >> r_shift;
                            break;
                        case 1:
                            p_value = (this->vec[i][vec_j] & AND_MASK_P1_6) >> r_shift;
                            break;
                        case 0:
                            p_value = (this->vec[i][vec_j] & AND_MASK_P1_7) >> r_shift;
                            break;
                        default:
                            ERROR_MSG("unexpected r_shift value in toBGRImage");
                            exit(1);
                            break;
                    }
                    break;

                case 2:
                    switch (r_shift) {
                        case 6:
                            p_value = (this->vec[i][vec_j] & AND_MASK_P2_0) >> r_shift;
                            break;
                        case 4:
                            p_value = (this->vec[i][vec_j] & AND_MASK_P2_1) >> r_shift;
                            break;
                        case 2:
                            p_value = (this->vec[i][vec_j] & AND_MASK_P2_2) >> r_shift;
                            break;
                        case 0:
                            p_value = (this->vec[i][vec_j] & AND_MASK_P2_3) >> r_shift;
                            break;
                        default:
                            ERROR_MSG("unexpected r_shift value in toBGRImage");
                            exit(1);
                            break;
                    }
                    break;

                case 4:
                    switch (r_shift) {
                        case 4:
                            p_value = (this->vec[i][vec_j] & AND_MASK_P4_0) >> r_shift;
                            break;
                        case 0:
                            p_value = (this->vec[i][vec_j] & AND_MASK_P4_1) >> r_shift;
                            break;
                        default:
                            ERROR_MSG("unexpected r_shift value in toBGRImage");
                            exit(1);
                            break;
                    }
                    break;
                
                default:
                    ERROR_MSG("invalid pixel_size value in toBGRImage");
                    exit(1);
                    break;

            }

            //TODO: why is the pixel set to a generic gray value?
            dst_img->at<cv::Vec3b>(i,j) = (p_value!=this->pixel_values) ? BGR_palette[p_value] : cv::Vec3b(0,0,0);

        }
    }

    return 0;

}



// ----------------------------------------------------------------------



Bitmap p2b::toBitmap(cv::Mat img, uint8_t pixel_size, const std::vector<uint8_t>& thresholds_v){
    
    uint8_t pixels_per_byte = 8/pixel_size;

    size_t bm_rows = img.rows;
    size_t bm_cols = img.cols;
    while ((bm_cols % pixels_per_byte) != 0){
        ++bm_cols;
    }
    bm_cols /= pixels_per_byte;

    Bitmap ret_bm = Bitmap(bm_rows, bm_cols, pixel_size, thresholds_v);
    ret_bm.fromImage(img);
    return ret_bm;

}



vector<vector<uint8_t>> p2b::toBits(cv::Mat img, uint8_t pixel_size, const std::vector<uint8_t>& thresholds_v){
    
    uint8_t pixels_per_byte = 8/pixel_size;

    size_t bm_rows = img.rows;
    size_t bm_cols = img.cols;
    while ((bm_cols % pixels_per_byte) != 0){
        ++bm_cols;
    }
    bm_cols /= pixels_per_byte;

    Bitmap bm = Bitmap(bm_rows, bm_cols, pixel_size, thresholds_v);
    bm.fromImage(img);
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



