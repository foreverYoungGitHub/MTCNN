cmake_minimum_required(VERSION 3.6)
project(FDDB_evaluation)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(OpenCV_DIR "/usr/local/lib")
find_package(OpenCV)

set(SOURCE_FILES
    evaluation/common.hpp
    evaluation/EllipseR.cpp
    evaluation/EllipseR.hpp
    evaluation/EllipsesSingleImage.cpp
    evaluation/EllipsesSingleImage.hpp
    evaluation/evaluate.cpp
    evaluation/Hungarian.cpp
    evaluation/Hungarian.hpp
    evaluation/Matching.cpp
    evaluation/Matching.hpp
    evaluation/MatchPair.cpp
    evaluation/MatchPair.hpp
    evaluation/OpenCVUtils.cpp
    evaluation/OpenCVUtils.hpp
    evaluation/RectangleR.cpp
    evaluation/RectangleR.hpp
    evaluation/RectanglesSingleImage.cpp
    evaluation/RectanglesSingleImage.hpp
    evaluation/Region.cpp
    evaluation/Region.hpp
    evaluation/RegionsSingleImage.cpp
    evaluation/RegionsSingleImage.hpp
    evaluation/Results.cpp
    evaluation/Results.hpp)

add_executable(FDDB_evaluation ${SOURCE_FILES})
target_link_libraries(FDDB_evaluation ${OpenCV_LIBS} )