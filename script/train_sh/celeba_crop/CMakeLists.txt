cmake_minimum_required(VERSION 3.6)
project(celeba_crop)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV)

include_directories( ${OpenCV_INCLUDE_DIRS} )

set(SOURCE_FILES main.cpp celeba_crop.cpp celeba_crop.h)
add_executable(celeba_crop ${SOURCE_FILES})

target_link_libraries(celeba_crop ${OpenCV_LIBS} )