#include <stdio.h>
#include <stdlib.h>
#include "VX/vx.h"
#include "VX/vxu.h"
#include "opencv-import.h"

/** @brief
test2 reads an image from a jpeg, imports it into an OpenVX file, and then
saves it back into a jpeg
*/
int main(int argc, char** argv)
{
  if(argc != 3)
  {
    printf("test2 reads an image from a jpeg, imports it into an OpenVX file,\n");
    printf("and then saves it back into a jpeg\n");
    printf("test <source image> <destination image>\n");
    exit(0);
  }

  vx_context context = vxCreateContext();
  vx_image image;

  if(vxa_read_image(argv[1], context, &image) != 1)
  {
    printf("Image reading failed\n");
    exit(0);
  }

  if(vxa_write_image(image, argv[2]) != 1)
  {
    printf("Image writing failed\n");
    exit(0);
  }
}
