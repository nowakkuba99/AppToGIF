#    _____              ___________      ________.______________
#   /  _  \ ______ _____\__    ___/___  /  _____/|   \_   _____/
#  /  /_\  \\____ \\____ \|    | /  _ \/   \  ___|   ||    __)  
# /    |    \  |_> >  |_> >    |(  <_> )    \_\  \   ||     \   
# \____|__  /   __/|   __/|____| \____/ \______  /___|\___  /   
#         \/|__|   |__|                        \/         \/ 
#
#
#                       Jakub Nowak 2023
#                  https://github.com/nowakkuba99

# This CMake project allows one to create static library AppToGIF

# CMake Version
cmake_minimum_required(VERSION 3.13)
# Project Name
project(AppToGIF)
# C++17 Standard
set(CMAKE_CXX_STANDARD 17)
# Source files paths
set(SOURCE_FILES 
    src/encoder/Encoder.cpp
    src/EncoderApp/EncoderApp.cpp
    src/Frame/Frame.cpp
    src/FrameBuffer/FrameBuffer.cpp)
# Add library
add_library(${CMAKE_PROJECT_NAME} STATIC ${SOURCE_FILES})

# --------------------------------------------------------------
# ----------------------- DEPENDENCIES -------------------------
# --------------------------------------------------------------
# To be also used in your project ...

# FFMPEG libraries
find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
find_library(AVCODEC_LIBRARY avcodec)

find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h)
find_library(AVFORMAT_LIBRARY avformat)

find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h)
find_library(AVUTIL_LIBRARY avutil)

find_path(AVDEVICE_INCLUDE_DIR libavdevice/avdevice.h)
find_library(AVDEVICE_LIBRARY avdevice)

find_path(SWSCALE_INCLUDE_DIR libswscale/swscale.h)
find_library(SWSCALE_LIBRARY swscale)

# Link libraries
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${AVCODEC_INCLUDE_DIR} ${AVFORMAT_INCLUDE_DIR} ${AVUTIL_INCLUDE_DIR} ${AVDEVICE_INCLUDE_DIR} ${SWSCALE_INCLUDE_DIR})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${AVCODEC_LIBRARY} ${AVFORMAT_LIBRARY} ${AVUTIL_LIBRARY} ${AVDEVICE_LIBRARY} ${SWSCALE_LIBRARY})