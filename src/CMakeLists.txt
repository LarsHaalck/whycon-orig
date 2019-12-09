project(whycon)
cmake_minimum_required(VERSION 3.5)

find_package(OpenCV REQUIRED)
if(OpenCV_VERSION VERSION_LESS "3.0")
    message(FATAL_ERROR "OpenCV version should be >= 3.0")
endif()


set(SOURCES
    main/whycon.cpp
    common/cmath.cpp
    common/CRawImage.cpp
    common/geometricMedian.cpp
    imageproc/CCircleDetect.cpp
)


add_executable(whycon ${SOURCES})

target_include_directories(whycon
    PRIVATE
    common
    imageproc
)

target_link_libraries(whycon
    PRIVATE
    opencv_core
    opencv_highgui
    opencv_imgproc
    opencv_videoio
    opencv_calib3d
)
