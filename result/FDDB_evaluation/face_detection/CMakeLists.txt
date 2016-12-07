cmake_minimum_required(VERSION 3.6)
project(face_detection)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(Caffe)
include_directories(${Caffe_INCLUDE_DIRS})

set(SOURCE_FILES main.cpp ../../../detection/MTCNN.cpp ../../../detection/MTCNN.h fddb_detection.cpp fddb_detection.h GenSample.cpp GenSample.h)
add_executable(face_detection ${SOURCE_FILES})

target_link_libraries(face_detection ${Caffe_LIBRARIES})