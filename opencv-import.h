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

/** @file opencv-import.h
@brief Functions for OpenVX I/O
*/

#ifndef _OPENCV_IMPORT_H
#define _OPENCV_IMPORT_H

#include "VX/vx.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Import a remap mapping from a file in OpenCV format (xml or yml)
 *
 * Imports a remap mapping from an xml in OpenCV format. Expects a
 * CV_16SC1 matrix that is saved in the OpenCV format.
 * @param filename Name of input file
 * @param nodename Name of the node that contains the matrix
 * @param context OpenVX context to import the mapping into
 * @param remap OpenVX remap object to import the mapping into
 * @param _dst_width Output parameter. If not NULL, contains the target
 * image width.
 * @param _dst_height Output parameter. If not NULL, contains the target
 * image height.
 * @return 1 if success
 */
int vxa_import_opencv_remap(const char* filename, const char* nodename,
  vx_context context, vx_remap* remap, int* _dst_width, int* _dst_height);

/**
 * @brief Import an image from a file in OpenCV format (xml or yml)
 *
 * Imports a matrix from an xml in OpenCV format and converts it into
 * an OpenVX image. Expects an CV_S16C1 image.
 * @param filename Name of the input file
 * @param nodename Name of the node that contains the matrix
 * @param context OpenVX context to import the mapping into
 * @param image A pointer to a OpenVX vx_image object. The function creates
 * the vx_image object and imports the matrix into it.
 * @param _dst_width Output parameter. If not NULL, contains the target
 * image width.
 * @param _dst_height Output parameter. If not NULL, contains the target
 * image height.
 * @return 1 if success
 */
int vxa_import_opencv_image(const char* filename, const char* nodename,
  vx_context context, vx_image* image, int* _dst_width,
  int* _dst_height);

/**
 * @brief Save an OpenVX image into a file
 *
 * Saves an OpenVX image into a file. OpenCV is used to write to a file,
 * so any format supported by OpenCV on your platform will be acceptable.
 * Expects an CV_S16C1 image.
 * @param img Input image
 * @param filename Name of the output file.
 * @return 1 if success, 0 otherwise
 */
int vxa_write_image(vx_image img, const char* filename);

/**
 * @brief Reads an OpenVX image from a file
 *
 * Reads an OpenVX image from a file. OpenCV is used to read from a file,
 * so any format supported by OpenCV on your platform will be acceptable.
 * Expects a VX_DF_IMAGE_U8 image.
 * @param filename Name of the input file.
 * @param context The OpenVX context
 * @param img Output OpenVX image created by this function.
 * @return 1 if success, 0 otherwise
 */
int vxa_read_image(const char* filename, vx_context context, vx_image* img);

/**
 * @brief Draws a set of lines
 *
 * Copies the input image to the output, and draws a set of lines given by
 * an array.
 * @param context An OpenVX context
 * @param image The input image, VX_DF_IMAGE_RGB or VX_DF_IMAGE_U8 is expected
 * @param lines An array of lines, containing vx_line2d_t structures
 * @param num_lines The number of lines to draw. Must be not greater than
 * the number of lines in the array
 * @param color Line color
 * @param thickness Line thickness
 * @param output A pointer to the output image, will be VX_DF_IMAGE_RGB
 * @return 1 if success, -1 otherwise
 */
int draw_lines(vx_context context, vx_image image, vx_array lines, vx_size num_lines,
  const vx_pixel_value_t* color, int thickness, vx_image* output);

  /**
   * @brief Draws a set of circles
   *
   * Copies the input image to the output, and draws a set of circles given by
   * an array.
   * @param context An OpenVX context
   * @param image The input image, VX_DF_IMAGE_RGB or VX_DF_IMAGE_U8 is expected
   * @param lines An array of circles, containing vx_coordinates2d_t structures
   * @param num_circles The number of circles to draw. Must be not greater than
   * the number of elements in the array
   * @param radius Circle radius
   * @param color Circle color
   * @param thickness Circle thickness
   * @param output A pointer to the output image, will be VX_DF_IMAGE_RGB
   * @return 1 if success, -1 otherwise
   */
  int draw_circles(vx_context context, vx_image image, vx_array circles, vx_size num_circles,
    int radius, const vx_pixel_value_t* color, int thickness, vx_image* output);

#ifdef __cplusplus
}
#endif

#endif /* _OPENCV_IMPORT_H */
