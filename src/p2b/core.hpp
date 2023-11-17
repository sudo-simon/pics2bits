#pragma once

#include "bitmap.hpp"

#include <cstddef>
#include <cstdint>
#include <opencv2/core/mat.hpp>
#include <vector>



// ----------------------------------------------------------------------



//? namespace declaration, p2b looks kinda cool ngl
namespace p2b {





/**
    @brief Transforms an OpenCV image in a p2b bitmap
    @param img_ptr: the input image read by OpenCV
    @param pixel_size: how many bits to use per pixel (1, 2 or 4)
    @param thresholds_v: the vector of uint8_t to use as thresholding to decide how to represent info
    @param parallel: boolean flag to perform parallel operations (default=true)
    @return the Bitmap object correctly initialized 
*/
Bitmap toBitmap(cv::Mat* img_ptr, uint8_t pixel_size, const std::vector<uint8_t>& thresholds_v, bool parallel=true);


/**
    @brief Transforms an OpenCV image in a matrix of bytes that follow the p2b rules
    @param img_ptr: the input image read by OpenCV
    @param pixel_size: how many bits to use per pixel (1, 2 or 4)
    @param thresholds_v: the vector of uint8_t to use as thresholding to decide how to represent info
    @param parallel: boolean flag to perform parallel operations (default=true)
    @return the matrix of uint8_t corresponding to the bitmap
*/
std::vector<std::vector<uint8_t>> toBits(cv::Mat* img_ptr, uint8_t pixel_size, const std::vector<uint8_t>& thresholds_v, bool parallel=true);


/**
    @brief Updates the content of the bitmap with the new image
    @param bitmap_ptr: the pointer to the bitmap object
    @param update_img_ptr: the pointer to the image to perform updating with
    @return 0 if ok, 1 otherwise
*/
int updateBitmap(Bitmap* bitmap_ptr, cv::Mat* update_img_ptr);


/**
    @brief Updates a specific region of the bitmap with the input image
    @param bitmap_ptr: the pointer to the bitmap object
    @param update_img_ptr: the pointer to the image to perform updating with
    @param start_row: the row index from which the region to update starts
    @param start_col: the col index from which the region to update starts
    @return 0 if ok, 1 otherwise
*/
int updateBitmapRegion(Bitmap* bitmap_ptr, cv::Mat* update_img_ptr, size_t start_row, size_t start_col);


/**
    @brief Core function to add an image to the bitmap following an "append" logic.
    The new image is added on the TOP (0), RIGHT (1), BOTTOM (2) or LEFT (3)
    of the last added portion of the bitmap in a contiguous fashion.
    Proper reallocation is performed accordingly to the new image size.
    @param bitmap_ptr: the pointer to the bitmap object
    @param add_img_ptr: the pointer to the image to add
    @param add_direction: int constant to indicate one of four directions (UP=0, RIGHT=1, DOWN=2, LEFT=3)
    @param minimal_resizing: boolean flag to activate minimal resizing of bitmap
    @return 0 if ok, 1 otherwise
*/
int addBits(Bitmap* bitmap_ptr, cv::Mat* add_img_ptr, const int add_direction, const bool minimal_resizing=false);







}   //? End of p2b namespace