#
# Global setup file for OpenVX CMake
#
cmake_minimum_required(VERSION 3.0)

project(opencv_import)
find_package(OpenCV REQUIRED)

if(${OpenCV_VERSION} VERSION_GREATER 4 OR ${OpenCV_VERSION} VERSION_EQUAL 4)
  message("OpenCV version 4.0 or higher detected")
  SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

if(${VXA_DEBUG} MATCHES "On")
  add_definitions(-DVXA_DEBUG)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
find_package(OpenVX)
include_directories(${OPENVX_INCLUDES} ${VXU_INCLUDES})

set(VXA_VERSION 0.1 CACHE STRING "version" FORCE)

set(include_dest "include/vxa")
set(main_lib_dest "lib")
set(headers vxa.h opencv-import.h opencv-import.hpp)

add_library(vxa SHARED opencv-import.cpp)
target_include_directories(vxa PUBLIC
  $<BUILD_INTERFACE:${MY_LIBRARY_SOURCE_DIR}> # for headers when building
	$<INSTALL_INTERFACE:${include_dest}> # for client in install mode
  )
target_link_libraries(vxa ${OPENVX_LIBRARIES} ${OpenCV_LIBS})

add_executable(vxa_test1 test1.c)
target_link_libraries(vxa_test1 ${OPENVX_LIBRARIES} vxa ${VXU_LIBRARIES} ${OpenCV_LIBS})

add_executable(vxa_test2 test2.c)
target_link_libraries(vxa_test2 ${OPENVX_LIBRARIES} vxa ${OpenCV_LIBS})

install(TARGETS vxa EXPORT vxa DESTINATION "${main_lib_dest}")
install(FILES ${headers} DESTINATION "${include_dest}")
install(FILES vxa-config.cmake DESTINATION ${main_lib_dest}/cmake/vxa)
install(EXPORT vxa DESTINATION "${main_lib_dest}/cmake/vxa")
