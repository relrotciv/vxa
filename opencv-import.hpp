/*
Copyright (c) 2018-2019 Victor Erukhimov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/** @file opencv-import.hpp
@brief Functions for OpenVX I/O to be used together with OpenCV
*/

#ifndef _OPENCV_IMPORT_HPP
#define _OPENCV_IMPORT_HPP

#ifndef __cplusplus
#pragma error("This header should be included in C++ sources only")
#endif

#include "VX/vx.h"
#include "opencv2/cv.hpp"

/**
 * @brief Convert an OpenVX image into an OpenCV image
 *
 * Converts an OpenVX image into an OpenCV image. Expects a
 * VX_DF_IMAGE_U8 or VX_DF_IMAGE_RGB image format.
 * @param vx_image OpenVX image
 * @param cv_image OpenCV image
 * @return 1 if success, -1 otherwise
 */
int vxa_vx2cv(vx_image vx_image, cv::Mat& cv_image);

#endif /* _OPENCV_IMPORT_HPP */
