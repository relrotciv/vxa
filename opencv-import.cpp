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

int vxa_write_image(vx_image image, const char* filename)
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
      printf("Format not supported\n");
      return(0);
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
    return(0);
  }

  Mat cv_img(height, width, cv_type);
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

  imwrite(filename, cv_img);

  return(1);
}

int vxa_read_image(const char* filename, vx_context context, vx_image* image)
{
  Mat cv_img = imread(filename);
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
      return(0);
  }

  int width = cv_img.cols, height = cv_img.rows;

  *image = vxCreateImage(context, width, height, img_type);

  vx_rectangle_t roi;
  roi.start_x = 0;
  roi.start_y = 0;
  roi.end_x = width;
  roi.end_y = height;

  vx_map_id map_id;

  vx_imagepatch_addressing_t addr;
  unsigned char* ptr;

  vx_status status = vxMapImagePatch(*image, &roi, 0, &map_id, &addr,
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

  vxUnmapImagePatch(*image, map_id);

  return(1);
}
