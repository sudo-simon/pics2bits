#include "include/pics2bits.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <vector>


//? ----------------------------------------------------------------------
//? PREPROCESSOR DEFINITIONS

//#define PIXEL_SIZE 2

#define OR_MASK_P1_0 0b01111111
#define OR_MASK_P1_1 0b10111111
#define OR_MASK_P1_2 0b11011111
#define OR_MASK_P1_3 0b11101111
#define OR_MASK_P1_4 0b11110111
#define OR_MASK_P1_5 0b11111011
#define OR_MASK_P1_6 0b11111101
#define OR_MASK_P1_7 0b11111110

#define OR_MASK_P2_0 0b00111111
#define OR_MASK_P2_1 0b11001111
#define OR_MASK_P2_2 0b11110011
#define OR_MASK_P2_3 0b11111100

//? ----------------------------------------------------------------------



#define PIXEL_TO_BINARY(byte)  \
    ((byte) & 0b00000010 ? '1' : '0'), \
    ((byte) & 0b00000001 ? '1' : '0')

#define DEBUG_MSG(msg) std::cout << "---- BITMAP DEBUG ----\n" << msg << "\n" << std::endl;


std::vector<int> AUX_sizeOfBitmaps(std::vector<std::vector<uint8_t>> v, uint8_t** p){
    int v_rows = v.capacity(), v_cols = v[0].capacity();
    int p_rows = 0, p_cols = 0;

    return {v_rows, v_cols, p_rows, p_cols};
}



Bitmap::Bitmap(unsigned int rows, unsigned int cols, uint8_t threshold, uint8_t pixel_size){
    if (rows <= 0 || cols <= 0){
        std::cerr << "ERROR: rows and cols must be >0" << std::endl;
        exit(1);
    }
    if (threshold < 0 || threshold > 255){
        std::cerr << "ERROR: threshold must be between 0 and 255" << std::endl;
        exit(1);
    }
    if ((pixel_size < 1) || (pixel_size > 8) || (pixel_size & (pixel_size-1)) != 0){
        std::cerr << "ERROR: pixel_size must be one of {1,2,4,8}" << std::endl;
        exit(1);
    }

    this->rows = rows;
    this->cols = cols;
    this->threshold = threshold;
    this->pixel_size = pixel_size;    //? Future implementation or separate source files?

    //? Vector implementation of the bitmap
    bitmap_v = std::vector<std::vector<uint8_t>>(rows, std::vector<uint8_t>(cols,255));
    //std::vector<std::vector<uint8_t>> tmp_vect(rows, std::vector<uint8_t>(cols,255));
    //bitmap_v = tmp_vect;

    //? Pointer implementation of the bitmap
    bitmap_p = (uint8_t**) malloc(rows*sizeof(uint8_t*));
    for (int i=0; i<this->rows; ++i){
        bitmap_p[i] = (uint8_t*) malloc(cols*sizeof(uint8_t));
        for (int j=0; j<this->cols; ++j){
            bitmap_p[i][j] = 255;
        }
    }
}



Bitmap::~Bitmap(){
    for (int i=0; i<rows; ++i){
        free(bitmap_p[i]);
    }
    free(bitmap_p);
}



int Bitmap::getRows(){ return rows; }
int Bitmap::getCols(){ return cols; }



//? Automatically doubles the sizes of the bitmap and initializes new elements to 0b11111111
//TODO The new size is not seen by the instance that calls the method, why?
int Bitmap::enlarge(){

    char msg[256] = "";
    snprintf(msg, 256, "Bitmap.enlarge() called:\n\trows = %d\n\tcols = %d",rows,cols);
    DEBUG_MSG(msg);

    rows *= 2;
    cols *= 2;

    bitmap_v.resize(rows, std::vector<uint8_t>(cols, 255));
    //for (std::vector<uint8_t> r : bitmap_v){
    //    r.resize(cols,255);
    //}

    /*
    bitmap_p = (uint8_t**) realloc(bitmap_p, rows*sizeof(uint8_t*));
    for (int i=0; i<rows; ++i){
        if (i < rows/2){
            bitmap_p[i] = (uint8_t*) realloc(bitmap_p[i], cols*sizeof(uint8_t));
            for (int j=cols/2; j<cols; ++j){
                bitmap_p[i][j] = 255;
            }
        }
        else{
            bitmap_p[i] = (uint8_t*) malloc(cols*sizeof(uint8_t));
            for (int j=0; j<cols; ++j){
                bitmap_p[i][j] = 255;
            }
        }
    }
    */

    return 0;
}



void Bitmap::print(){
    std::string out = "";
    char* tmp_s = (char*) malloc(4);
    uint8_t p0, p1, p2, p3;

    out += "---> Vector implementation:\n\n";
    for (std::vector<uint8_t> r : bitmap_v){
        out += "| ";
        for (uint8_t cell : r){
            p0 = (cell & 0b11000000) >> 6;
            std::snprintf(tmp_s, 4, "%c%c ", PIXEL_TO_BINARY(p0));
            out += tmp_s;
            p1 = (cell & 0b00110000) >> 4;
            std::snprintf(tmp_s, 4, "%c%c ", PIXEL_TO_BINARY(p1));
            out += tmp_s;
            p2 = (cell & 0b00001100) >> 2;
            std::snprintf(tmp_s, 4, "%c%c ", PIXEL_TO_BINARY(p2));
            out += tmp_s;
            p3 = (cell & 0b00000011);
            std::snprintf(tmp_s, 4, "%c%c ", PIXEL_TO_BINARY(p3));
            out += tmp_s;
            out += "| ";
        }
        out += "\n";
    }
    
    out += "\n---> Pointer implementation:\n\n";
    for (int i=0; i<rows; ++i){
        out += "| ";
        for (int j=0; j<cols; ++j){
            p0 = (bitmap_p[i][j] & 0b11000000) >> 6;
            std::snprintf(tmp_s, 4, "%c%c ", PIXEL_TO_BINARY(p0));
            out += tmp_s;
            p1 = (bitmap_p[i][j] & 0b00110000) >> 4;
            std::snprintf(tmp_s, 4, "%c%c ", PIXEL_TO_BINARY(p1));
            out += tmp_s;
            p2 = (bitmap_p[i][j] & 0b00001100) >> 2;
            std::snprintf(tmp_s, 4, "%c%c ", PIXEL_TO_BINARY(p2));
            out += tmp_s;
            p3 = (bitmap_p[i][j] & 0b00000011);
            std::snprintf(tmp_s, 4, "%c%c ", PIXEL_TO_BINARY(p3));
            out += tmp_s;
            out += "| ";
        }
        out += "\n";
    }

    std::cout << out << std::endl;
    free(tmp_s);
}



int Bitmap::fromImage(cv::Mat img){
    int img_rows = img.rows;
    int img_cols = img.cols;

    char msg[256] = "";

    std::vector<int> s = AUX_sizeOfBitmaps(bitmap_v, bitmap_p);
    snprintf(
        msg,
        256,
        "bitmap_v dimensions when fromImage() called = %dx%d\n",
        s[0], s[1]
    );
    DEBUG_MSG(msg);

    cv::Mat gs_img = img;
    if (img.channels() > 1)
        cv::cvtColor(img, gs_img, cv::COLOR_BGR2GRAY);

    unsigned int bitmap_j;
    uint8_t p_value;
    uint8_t l_shift;

    snprintf(msg, 256, "rows = %d\ncols = %d", rows, cols);
    DEBUG_MSG(msg);
    snprintf(msg, 256, "img_rows = %d\nimg_cols = %d",img_rows,img_cols);
    DEBUG_MSG(msg);

    for (int i=0; i<img_rows; ++i){
        for (int j=0; j<img_cols; ++j){
            bitmap_j = j/(8/pixel_size);

            /*
            snprintf(
                msg,
                256,
                "[%d,%d] - bitmap_j = %d",
                i,j, bitmap_j
            );
            DEBUG_MSG(msg);
            */

            //? When to set pixel to 1, future implementation //TODO
            if (gs_img.at<uint8_t>(i,j) > threshold) p_value = 0;
            else p_value = 1;

            //? Actual tricky part :)
            l_shift = (8-pixel_size)-((j%(8/pixel_size))*pixel_size);

            if (pixel_size == 2){
                switch (l_shift) {
                    case 6:
                        bitmap_v[i][bitmap_j] &= ((p_value << l_shift) | OR_MASK_P2_0);
                        //bitmap_p[i][bitmap_j] &= ((p_value << l_shift) | OR_MASK_P2_0);
                    case 4:
                        bitmap_v[i][bitmap_j] &= ((p_value << l_shift) | OR_MASK_P2_1);
                        //bitmap_p[i][bitmap_j] &= ((p_value << l_shift) | OR_MASK_P2_1);
                    case 2:
                        bitmap_v[i][bitmap_j] &= ((p_value << l_shift) | OR_MASK_P2_2);
                        //bitmap_p[i][bitmap_j] &= ((p_value << l_shift) | OR_MASK_P2_2);
                    case 0:
                        bitmap_v[i][bitmap_j] &= ((p_value << l_shift) | OR_MASK_P2_3);
                        //bitmap_p[i][bitmap_j] &= ((p_value << l_shift) | OR_MASK_P2_3);
                }
            }

            else if (pixel_size == 1){
                switch (l_shift) {
                    case 7:
                        bitmap_v[i][bitmap_j] &= ((p_value << l_shift) | OR_MASK_P1_0);
                        bitmap_p[i][bitmap_j] &= ((p_value << l_shift) | OR_MASK_P1_0);
                    case 6:
                        bitmap_p[i][bitmap_j] &= ((p_value << l_shift) | OR_MASK_P1_1);
                        bitmap_v[i][bitmap_j] &= ((p_value << l_shift) | OR_MASK_P1_1);
                    case 5:
                        bitmap_v[i][bitmap_j] &= ((p_value << l_shift) | OR_MASK_P1_2);
                        bitmap_p[i][bitmap_j] &= ((p_value << l_shift) | OR_MASK_P1_2);
                    case 4:
                        bitmap_v[i][bitmap_j] &= ((p_value << l_shift) | OR_MASK_P1_3);
                        bitmap_p[i][bitmap_j] &= ((p_value << l_shift) | OR_MASK_P1_3);
                    case 3:
                        bitmap_v[i][bitmap_j] &= ((p_value << l_shift) | OR_MASK_P1_4);
                        bitmap_p[i][bitmap_j] &= ((p_value << l_shift) | OR_MASK_P1_4);
                    case 2:
                        bitmap_v[i][bitmap_j] &= ((p_value << l_shift) | OR_MASK_P1_5);
                        bitmap_p[i][bitmap_j] &= ((p_value << l_shift) | OR_MASK_P1_5);
                    case 1:
                        bitmap_v[i][bitmap_j] &= ((p_value << l_shift) | OR_MASK_P1_6);
                        bitmap_p[i][bitmap_j] &= ((p_value << l_shift) | OR_MASK_P1_6);
                    case 0:
                        bitmap_v[i][bitmap_j] &= ((p_value << l_shift) | OR_MASK_P1_7);
                        bitmap_p[i][bitmap_j] &= ((p_value << l_shift) | OR_MASK_P1_7);
                }
            }
        }
    }

    return 0;
}



int Bitmap::fromGrayscaleImage(cv::Mat gs_img){
    int img_rows = gs_img.rows, img_cols = gs_img.cols;
    unsigned int bitmap_j;
    uint8_t p_value, l_shift;

    for (int i=0; i<img_rows; ++i){
        for (int j=0; j<img_cols; ++j){
            bitmap_j = j/4;
            if (gs_img.at<uint8_t>(i,j) > threshold) p_value = 1;
            else p_value = 0;

            //? Actual tricky part :)
            l_shift = 6-((j%4)*2);
            switch (l_shift) {
                case 6:
                    bitmap_v[i][bitmap_j] &= ((p_value << l_shift) | 0b00111111);
                    bitmap_p[i][bitmap_j] &= ((p_value << l_shift) | 0b00111111);
                case 4:
                    bitmap_v[i][bitmap_j] &= ((p_value << l_shift) | 0b11001111);
                    bitmap_p[i][bitmap_j] &= ((p_value << l_shift) | 0b11001111);
                case 2:
                    bitmap_v[i][bitmap_j] &= ((p_value << l_shift) | 0b11110011);
                    bitmap_p[i][bitmap_j] &= ((p_value << l_shift) | 0b11110011);
                case 0:
                    bitmap_v[i][bitmap_j] &= ((p_value << l_shift) | 0b11111100);
                    bitmap_p[i][bitmap_j] &= ((p_value << l_shift) | 0b11111100);
            }
        }
    }

    return 0;
}



std::vector<cv::Mat> Bitmap::toImages(){
    cv::Mat ret_v = cv::Mat(rows,cols*(8/pixel_size),CV_8U);
    cv::Mat ret_p = cv::Mat(rows,cols*(8/pixel_size),CV_8U);

    for (int i=0; i<rows; ++i){
        for (int j=0; j<cols*(8/pixel_size); ++j){
            switch (bitmap_v[i][j/(8/pixel_size)]) {
                case 0b00: ret_v.at<uint8_t>(i,j) = 255;
                case 0b01: ret_v.at<uint8_t>(i,j) = 150;
                case 0b11: ret_v.at<uint8_t>(i,j) = 0;
            }
            //switch (bitmap_p[i][j/(8/pixel_size)]) {
            //    case 0b00: ret_p.at<uint8_t>(i,j) = 255;
            //    case 0b01: ret_p.at<uint8_t>(i,j) = 150;
            //    case 0b11: ret_p.at<uint8_t>(i,j) = 0;
            //}
        }
    }
    
    return {ret_v,ret_p};
}



std::vector<long int> Bitmap::sizeInMemory(){
    long int v_size = sizeof(bitmap_v);
    long int p_size = sizeof(bitmap_p);
    for (int i=0; i<rows; ++i){
        v_size += sizeof(bitmap_v[i]);
        p_size += sizeof(bitmap_p[i]);
        for (int j=0; j<cols; ++j){
            v_size += sizeof(bitmap_v[i][j]);
            p_size += sizeof(bitmap_p[i][j]);
        }
    }
    return {v_size, p_size};
}