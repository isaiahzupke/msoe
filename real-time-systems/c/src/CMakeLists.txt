cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

# This is the name of the project
project(PiImageStreamer LANGUAGES CXX)

set( CMAKE_VERBOSE_MAKEFILE on )

#Obtain the openCV package 
find_package( OpenCV REQUIRED )
find_library( libmmal REQUIRED)

# This defines system level include directories, where headers for system libraries are defined.
include_directories(SYSTEM /rpi_sysroot/usr/local/include /rpi_sysroot/usr/include /rpi_sysroot/opt/vc/include)

# This provides additional compile options.
add_definitions(-Wall -g -O0 -L/rpi_sysroot/usr/local/lib -lwiringPi -lpthread)

# This identifies the source code files that are relevant to the project.
file(GLOB SOURCES "*.cpp")



# Find the doxygen tool
find_package(Doxygen)

include(UseDoxygenDoc)

add_doxygen_doc(
  BUILD_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/../docs/
  DOXY_FILE
    ${CMAKE_CURRENT_SOURCE_DIR}/../docs/Doxyfile.in
  TARGET_NAME
    docs
  COMMENT
    "HTML documentation"
  )

link_directories( /rpi_sysroot/opt/vc/lib /rpi_sysroot/usr/lib 
/rpi_sysroot/usr/lib/arm-linux-gnueabihf ) # for specific path

# This defines that an executable is to be created.
add_executable(PiImageStreamer ${SOURCES})

# This defines the libraries which are needed by this project for it to link properly.
target_link_libraries(PiImageStreamer   ${OpenCV_LIBS} )
target_link_libraries(PiImageStreamer    wiringPi)
target_link_libraries(PiImageStreamer   pthread )
target_link_libraries(PiImageStreamer   rt )
target_link_libraries(PiImageStreamer   mmal_core )
target_link_libraries(PiImageStreamer   mmal_util )
target_link_libraries(PiImageStreamer   mmal_vc_client )
target_link_libraries(PiImageStreamer   vcos  )
target_link_libraries(PiImageStreamer   bcm_host )
target_link_libraries(PiImageStreamer   vchiq_arm )
target_link_libraries(PiImageStreamer   vcsm )
target_link_libraries(PiImageStreamer /rpi_sysroot/usr/lib/arm-linux-gnueabihf/blas/libblas.so.3 )
target_link_libraries(PiImageStreamer /rpi_sysroot//usr/lib/arm-linux-gnueabihf/lapack/liblapack.so.3)



