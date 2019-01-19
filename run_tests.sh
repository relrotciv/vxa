#!/bin/sh

set +
if  [ "$#" -ne 1 ]; then
    echo "Run this script with the command: ./run_tests <path to built binaries"
    exit
fi

if [ ! -d test_output ]; then
  mkdir test_output
fi
$1/vxa_test1 test_data/test1.xml test_image_s16 test_output/test1_output.jpg
$1/vxa_test2 test_data/test.jpg test_output/test2_output.jpg
