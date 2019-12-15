cmake_minimum_required(VERSION 3.14)
project(Image-Classifier)


set(CMAKE_CXX_STANDARD 14)
find_package(OpenCV REQUIRED)

include_directories(include ${OpenCV_INCLUDE_DIRS})


add_executable(Image-Classifier
        src/main.cpp
        src/histogram.cpp include/histogram.hpp
        src/square_detector.cpp include/square_detector.hpp
        src/tools.cpp include/tools.hpp
        src/Drawing.cpp include/Drawing.h
        src/match_pattern.cpp include/match_pattern.h)

target_link_libraries(Image-Classifier ${OpenCV_LIBS})

