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

#include "opencv-import.h"
#include "opencv2/opencv.hpp"
#include <string>

using namespace cv;

int vxa_import_opencv_remap(const char* filename, const char* nodename,
  vx_context context, vx_remap* remap, int* _dst_width, int* _dst_height)
{
  FileStorage fs(filename, FileStorage::READ);

  Mat cv_remap;
  fs[nodename] >> cv_remap;
  int src_width, src_height, dst_width, dst_height;
  fs[(std::string(nodename) + std::string("_src_width")).c_str()] >> src_width;
  fs[(std::string(nodename) + std::string("_src_height")).c_str()] >> src_height;
  fs[(std::string(nodename) + std::string("_dst_width")).c_str()] >> dst_width;
  fs[(std::string(nodename) + std::string("_dst_height")).c_str()] >> dst_height;

  if(_dst_width != NULL)
  {
    *_dst_width = dst_width;
  }
  if(_dst_height != NULL)
  {
    *_dst_height = dst_height;
  }

  vx_rectangle_t roi;
  roi.start_x = 0;
  roi.start_y = 0;
  roi.end_x = cv_remap.cols;
  roi.end_y = cv_remap.rows;

  *remap = vxCreateRemap(context, src_width, src_height,
    dst_width, dst_height);

  vxCopyRemapPatch(*remap, &roi, cv_remap.step, cv_remap.ptr(),
    VX_TYPE_COORDINATES2DF, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);

  return(1);
}

int vxa_import_opencv_image(const char* filename, const char* nodename,
  vx_context context, vx_image* image, int* _dst_width, int* _dst_height)
{
  FileStorage fs(filename, FileStorage::READ);
  Mat cv_img;
  fs[nodename] >> cv_img;

  *image = vxCreateImage(context, cv_img.cols, cv_img.rows,
    VX_DF_IMAGE_S16);

  vx_rectangle_t roi;
  roi.start_x = 0;
  roi.start_y = 0;
  roi.end_x = cv_img.cols;
  roi.end_y = cv_img.rows;

  vx_imagepatch_addressing_t addr;
  addr.dim_x = roi.start_x;
  addr.dim_y = roi.start_y;
  addr.stride_x = 2;
  addr.stride_y = cv_img.step;
  vx_status status = vxCopyImagePatch(*image, &roi, 0, &addr, cv_img.ptr(0),
    VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);
  if(status != VX_SUCCESS)
  {
    printf("vxCopyImagePatch returned error with code %d\n", status);
    return -1;
  }

  if(_dst_width != NULL)
  {
    *_dst_width = cv_img.cols;
  }

  if(_dst_height != NULL)
  {
    *_dst_height = cv_img.rows;
  }

  return(1);
}

int vxa_vx2cv(vx_image image, cv::Mat& cv_img)
{
  int width, height;
  vxQueryImage(image, VX_IMAGE_WIDTH, &width, 4);
  vxQueryImage(image, VX_IMAGE_HEIGHT, &height, 4);

  vx_df_image img_type;
  vxQueryImage(image, VX_IMAGE_FORMAT, &img_type, 4);
  int cv_type;
  switch(img_type)
  {
    case VX_DF_IMAGE_U8:
      cv_type = CV_8UC1;
      break;

    case VX_DF_IMAGE_RGB:
      cv_type = CV_8UC3;
      break;

    default:
      printf("Format %d not supported\n", img_type);
      return(-1);
  }

  vx_rectangle_t roi;
  roi.start_x = 0;
  roi.start_y = 0;
  roi.end_x = width;
  roi.end_y = height;

  vx_map_id map_id;

  vx_imagepatch_addressing_t addr;
  unsigned char* ptr;

  vx_status status = vxMapImagePatch(image, &roi, 0, &map_id, &addr,
    (void**)&ptr, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, VX_NOGAP_X);
  if(status != VX_SUCCESS)
  {
    printf("vxMapImagePatch returned error with code %d\n", status);
    return(-1);
  }

  cv_img = cv::Mat(height, width, cv_type);
  if(addr.stride_x != 1 && addr.stride_x != 3)
  {
    printf("addressing structure not supported, stride_x = %d\n",
      addr.stride_x);
    return(0);
  }

  for(int y = 0; y < height; y++)
  {
    unsigned char* ptr_y = ptr + y*addr.stride_y;
    memcpy(cv_img.ptr(y), ptr_y, addr.stride_y);
  }

  vxUnmapImagePatch(image, map_id);

  return(1);
}

vx_image vxa_cv2vx(cv::Mat& cv_img, vx_context context)
{
  vx_df_image img_type;
  switch(cv_img.type())
  {
    case CV_8UC1:
      img_type = VX_DF_IMAGE_U8;
      break;

    case CV_8UC3:
      img_type = VX_DF_IMAGE_RGB;
      break;

    default:
      return(NULL);
  }

  int width = cv_img.cols, height = cv_img.rows;

  vx_image image = vxCreateImage(context, width, height, img_type);

  vx_rectangle_t roi;
  roi.start_x = 0;
  roi.start_y = 0;
  roi.end_x = width;
  roi.end_y = height;

  vx_map_id map_id;

  vx_imagepatch_addressing_t addr;
  unsigned char* ptr;

  vx_status status = vxMapImagePatch(image, &roi, 0, &map_id, &addr,
    (void**)&ptr, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST, VX_NOGAP_X);
  if(status != VX_SUCCESS)
  {
    printf("vxMapImagePatch returned error with code %d\n", status);
    return(0);
  }

  for(int y = 0; y < height; y++)
  {
    unsigned char* ptr_y = ptr + y*addr.stride_y;
    memcpy(ptr_y, cv_img.ptr(y), addr.stride_y);
  }

  vxUnmapImagePatch(image, map_id);

  return image;
}

int vxa_write_image(vx_image image, const char* filename)
{
  cv::Mat cv_img;
  vxa_vx2cv(image, cv_img);
  imwrite(filename, cv_img);

  return(1);
}

int vxa_read_image(const char* filename, vx_context context, vx_image* image)
{
  Mat cv_img = imread(filename);
  *image = vxa_cv2vx(cv_img, context);

  return(1);
}

int draw_lines(vx_context context, vx_image image, vx_array lines, vx_size num_lines,
  const vx_pixel_value_t* color, int thickness, vx_image* output)
{
  /* both input and output images should be RGB */
  vx_df_image img_type;
  vxQueryImage(image, VX_IMAGE_FORMAT, &img_type, 4);
  if(img_type != VX_DF_IMAGE_RGB && img_type != VX_DF_IMAGE_U8)
  {
    return(-1);
  }

  // convert to an opencv image
  cv::Mat _cv_img, cv_img;
  vxa_vx2cv(image, _cv_img);
  switch(img_type)
  {
  case VX_DF_IMAGE_RGB:
    cv_img = _cv_img;
    break;

  case VX_DF_IMAGE_U8:
    cv::cvtColor(_cv_img, cv_img, CV_GRAY2RGB);
    break;

  default:
    return(-1);
  }

  // allocate buffer for array data
  vx_line2d_t* _lines = new vx_line2d_t[num_lines];

  // copy data from array
  vxCopyArrayRange(lines, 0, num_lines, sizeof(vx_line2d_t), _lines,
    VX_READ_ONLY, VX_MEMORY_TYPE_HOST);

  cv::Scalar _color = CV_RGB(color->RGB[0], color->RGB[1],
    color->RGB[2]);

  // draw the lines with OpenCV
  for(int i = 0; i < num_lines; i++)
  {
    cv::Point pt1(_lines[i].start_x, _lines[i].start_y);
    cv::Point pt2(_lines[i].end_x, _lines[i].end_y);
    cv::line(cv_img, pt1, pt2, _color, thickness);
  }

  // convert back to vx_image
  *output = vxa_cv2vx(cv_img, context);

  delete[] _lines;
  return 1;
}

int draw_circles(vx_context context, vx_image image, vx_array centers, vx_size num_circles,
  int radius, const vx_pixel_value_t* color, int thickness, vx_image* output)
{
  /* both input and output images should be RGB */
  vx_df_image img_type;
  vxQueryImage(image, VX_IMAGE_FORMAT, &img_type, 4);
  if(img_type != VX_DF_IMAGE_RGB && img_type != VX_DF_IMAGE_U8)
  {
    return(-1);
  }

  // convert to an opencv image
  cv::Mat _cv_img, cv_img;
  vxa_vx2cv(image, _cv_img);
  switch(img_type)
  {
  case VX_DF_IMAGE_RGB:
    cv_img = _cv_img;
    break;

  case VX_DF_IMAGE_U8:
    cv::cvtColor(_cv_img, cv_img, CV_GRAY2RGB);
    break;

  default:
    return(-1);
  }

  // allocate buffer for array data
  vx_coordinates2d_t* _centers = new vx_coordinates2d_t[num_circles];

  // copy data from array
  vxCopyArrayRange(centers, 0, num_circles, sizeof(vx_coordinates2d_t), _centers,
    VX_READ_ONLY, VX_MEMORY_TYPE_HOST);

  cv::Scalar _color = CV_RGB(color->RGB[0], color->RGB[1],
    color->RGB[2]);

  // draw the lines with OpenCV
  for(int i = 0; i < num_circles; i++)
  {
    cv::Point pt(_centers[i].x, _centers[i].y);
    cv::circle(cv_img, pt, radius, _color, thickness);
  }

  // convert back to vx_image
  *output = vxa_cv2vx(cv_img, context);

  delete[] _centers;
  return 1;
}
