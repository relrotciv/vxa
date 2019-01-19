#
# Global setup file for OpenVX CMake

#
cmake_minimum_required(VERSION 3.0)

project(opencv_import)
find_package(OpenCV REQUIRED)
find_library(OPENVX openvx)

if(${OpenCV_VERSION} VERSION_GREATER 4 OR ${OpenCV_VERSION} VERSION_EQUAL 4)
  message("OpenCV version 4.0 or higher detected")
  SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

include_directories($ENV{C_INCLUDE_PATH})

add_library(opencv_import SHARED
  opencv-import.cpp)
target_link_libraries(opencv_import ${OPENVX} ${OpenCV_LIBS})

add_executable(vxa_test1 test1.c)
target_link_libraries(vxa_test1 ${OPENVX} opencv_import vxu ${OpenCV_LIBS})

add_executable(vxa_test2 test2.c)
target_link_libraries(vxa_test2 ${OPENVX} opencv_import vxu ${OpenCV_LIBS})
