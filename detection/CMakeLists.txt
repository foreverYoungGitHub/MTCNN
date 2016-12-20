cmake_minimum_required(VERSION 3.6)
project(MTCNN)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV)

find_package(Caffe)
#message(FATAL_ERROR ${Caffe_INCLUDE_DIRS})
include_directories(${Caffe_INCLUDE_DIRS})

set(SOURCE_FILES main.cpp MTCNN.cpp MTCNN.h)
add_executable(MTCNN ${SOURCE_FILES})

target_link_libraries(MTCNN ${OpenCV_LIBS} )
target_link_libraries(MTCNN ${Caffe_LIBRARIES})