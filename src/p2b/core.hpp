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
* @brief Transforms an OpenCV image in a p2b bitmap
* @param cv::Mat img - the input image read by OpenCV
* @param uint_8 pixel_size - how many bits to use per pixel (1, 2 or 4)
* @param uint_8 threshold - the 0<value<256 to use as the thresholding to decide how to represent info
* @return Bitmap - The Bitmap object correctly initialized 
*/
Bitmap toBitmap(cv::Mat* img_ptr, uint8_t pixel_size, const std::vector<uint8_t>& thresholds_v, bool parallel=true);



std::vector<std::vector<uint8_t>> toBits(cv::Mat* img_ptr, uint8_t pixel_size, const std::vector<uint8_t>& thresholds_v, bool parallel=true);



int updateBitmap(Bitmap* bitmap_ptr, cv::Mat* update_img_ptr);



int updateBitmapRegion(Bitmap* bitmap_ptr, cv::Mat* update_img_ptr, size_t start_row, size_t start_col);


/*
    Core function to add an image to the bitmap following an "append" logic.
    The new image is added on the TOP (0), RIGHT (1), BOTTOM (2) or LEFT (3)
    of the last added portion of the bitmap in a contiguous fashion.
    Proper reallocation is performed accordingly to the new image size.
*/
int addBits(Bitmap* bitmap_ptr, cv::Mat* add_img_ptr, int add_direction, bool minimal_resizing=false);



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