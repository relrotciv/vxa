#include <stdio.h>
#include <stdlib.h>
#include "VX/vx.h"
#include "VX/vxu.h"
#include "opencv-import.h"

/** @brief
test1 reads an s16 image saved by OpenCV in an xml/yml file, imports
it into OpenVX image, converts to U8 and saves to a file.
*/
int main(int argc, char** argv)
{
  if(argc != 4)
  {
    printf("This test reads an s16 image saved by OpenCV in an xml/yml file,\n");
    printf("imports it into OpenVX image, converts to U8 and saves to a file.\n");
    printf("test1 <source file xml> <node name> <output jpg>\n");
    exit(0);
  }

  const char* input_filename = argv[1];
  const char* nodename = argv[2];
  const char* output_filename = argv[3];

  vx_context context = vxCreateContext();
  vx_image imgs16, imgu8;

  int width, height;
  vxa_import_opencv_image(input_filename, nodename, context,
    &imgs16, &width, &height);

  imgu8 = vxCreateImage(context, width, height, VX_DF_IMAGE_U8);

  int _width;
  vxQueryImage(imgs16, VX_IMAGE_WIDTH, &_width, 4);

  vx_status status = vxuConvertDepth(context, imgs16, imgu8,
    VX_CONVERT_POLICY_SATURATE, 0);
  if(status != VX_SUCCESS)
  {
    printf("vxuConvertDepth returned error with code %d\n", status);
    exit(0);
  }
  vxa_write_image(imgu8, output_filename);

  vxReleaseContext(&context);
}
