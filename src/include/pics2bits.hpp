#pragma once

#include <cstddef>
#include <cstdint>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/matx.hpp>
#include <vector>

// ----------------------------------------------------------------------


#define DEBUG_MSG(msg) std::cout << "---- P2B DEBUG ----\n" << msg << std::endl;
#define ERROR_MSG(msg) std::cerr << "---- P2B ERROR ----\n" << msg << std::endl;


// ----------------------------------------------------------------------





//? namespace declaration, p2b looks kinda cool ngl
namespace p2b {


//? Bit masks to perform bitwise OR operations
const uint8_t OR_MASK_P1_0 = 0b01111111;
const uint8_t OR_MASK_P1_1 = 0b10111111;
const uint8_t OR_MASK_P1_2 = 0b11011111;
const uint8_t OR_MASK_P1_3 = 0b11101111;
const uint8_t OR_MASK_P1_4 = 0b11110111;
const uint8_t OR_MASK_P1_5 = 0b11111011;
const uint8_t OR_MASK_P1_6 = 0b11111101;
const uint8_t OR_MASK_P1_7 = 0b11111110;

const uint8_t OR_MASK_P2_0 = 0b00111111;
const uint8_t OR_MASK_P2_1 = 0b11001111;
const uint8_t OR_MASK_P2_2 = 0b11110011;
const uint8_t OR_MASK_P2_3 = 0b11111100;

const uint8_t OR_MASK_P4_0 = 0b00001111;
const uint8_t OR_MASK_P4_1 = 0b11110000;

//? Bit masks to perform bitwise AND operations
const uint8_t AND_MASK_P1_0 = 0b10000000;
const uint8_t AND_MASK_P1_1 = 0b01000000;
const uint8_t AND_MASK_P1_2 = 0b00100000;
const uint8_t AND_MASK_P1_3 = 0b00010000;
const uint8_t AND_MASK_P1_4 = 0b00001000;
const uint8_t AND_MASK_P1_5 = 0b00000100;
const uint8_t AND_MASK_P1_6 = 0b00000010;
const uint8_t AND_MASK_P1_7 = 0b00000001;

const uint8_t AND_MASK_P2_0 = 0b11000000;
const uint8_t AND_MASK_P2_1 = 0b00110000;
const uint8_t AND_MASK_P2_2 = 0b00001100;
const uint8_t AND_MASK_P2_3 = 0b00000011;

const uint8_t AND_MASK_P4_0 = 0b11110000;
const uint8_t AND_MASK_P4_1 = 0b00001111;


//? Constants used to initialize Bitmap objects
//const int BITMAP_TYPE_THRESHOLD = 0;


//? Constants used by the addBits function
const int ADD_UP = 0;
const int ADD_RIGHT = 1;
const int ADD_DOWN = 2;
const int ADD_LEFT = 3;



/**
* @brief The Bitmap class used to store informations, what the library revolves around
*/
class Bitmap{

    private:

        /*
        unsigned or int? Significant security issues involving overflows?
        */
        size_t rows;
        size_t cols;
        uint8_t pixel_size;
        uint8_t pixels_per_byte;
        uint8_t pixel_values;
        std::vector<uint8_t> thresholds_v;

        /*TODO
        We can include values, such as
            unsigned square_size
            unsigned last_added_row
            unsigned last_added_col
        to implement a bitmap that keeps in memory what is the last square it
        has added to itself (i and j) and proceed from there. It can use the square_size 
        value to estabilish the correct indexes of the square and the next one to add.
        */

        std::vector<std::vector<uint8_t>> vec;

    public:

        Bitmap();
        Bitmap(size_t rows, size_t cols, uint8_t pixel_size, const std::vector<uint8_t>& thresholds_v);
        //~Bitmap();
        size_t getRows();
        size_t getCols();
        std::vector<std::vector<uint8_t>> getVec();
        int increaseSize(size_t new_rows, size_t new_cols);
        int doubleSize();
        int fromImage(cv::Mat img);
        //int fromImage(cv::Mat img, int bitmap_type);
        int toGrayscaleImage(cv::Mat* dst_img, const std::vector<uint8_t>& grayscale_palette);
        int toBGRImage(cv::Mat* dst_img, const std::vector<cv::Vec3b>& color_palette);

};



/**
* @brief Transforms an OpenCV image in a p2b bitmap
* @param cv::Mat img - the input image read by OpenCV
* @param uint_8 pixel_size - how many bits to use per pixel (1, 2 or 4)
* @param uint_8 threshold - the 0<value<256 to use as the thresholding to decide how to represent info
* @return Bitmap - The Bitmap object correctly initialized 
*/
Bitmap toBitmap(cv::Mat img, uint8_t pixel_size, const std::vector<uint8_t>& thresholds_v);



std::vector<std::vector<uint8_t>> toBits(cv::Mat img, uint8_t pixel_size, const std::vector<uint8_t>& thresholds_v);



int addBits(Bitmap* bitmap_p, cv::Mat add_img, int add_direction);



int updateBitmap(Bitmap* bitmap_p, cv::Mat updated_img);



/*
How to use this?
We could create a new stitch every delta_t and use the newly formed image to increase 
the size of the bitmap to the correct rows and cols. Then use the diference between
the new and the previous image to have the bitmap updated correctly via the addBits function.

Our own stitching algorithm should be implemented (nothing more than some additions to RANSAC) 
to be able to identify the direction in which we are adding a new "square" and also being able to
create stitches consisting of multiple "squares" on a grid, with the possibility of 
leaving empty contiguous spaces and not creating a "panorama style" image.

This to allow for an efficient addBits function and a grid-like visualization of the resulting image,
complementing the matrix structure of the bitmap well.

Another option is to implement the updateBitmap function so that it takes just the new stitched 
image, computes the difference between this image (probably a thresholded, grayscale frame with objects of 
interest all in the same color via object detection) and the currently allocated bitmap, using uninitialized 
pixels as a mask of what's new. This computed difference is then used to initialize correct pixels to correct values, 
isolating the operations only to the region to update and not the whole image again, removing useless 
and costly operations.
*/






}   //? End of p2b namespace