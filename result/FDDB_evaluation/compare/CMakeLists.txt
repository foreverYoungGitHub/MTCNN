cmake_minimum_required(VERSION 3.6)
project(compare)

set(CMAKE_CXX_STANDARD 11)

set(OpenCV_DIR "/usr/local/lib")
find_package(OpenCV)

set(SOURCE_FILES main.cpp compare.cpp compare.h)
add_executable(compare ${SOURCE_FILES})
target_link_libraries(compare ${OpenCV_LIBS} )