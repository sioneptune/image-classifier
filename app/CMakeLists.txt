cmake_minimum_required(VERSION 3.14)
project(Image-Classifier)


set(CMAKE_CXX_STANDARD 14)
find_package(OpenCV REQUIRED)

include_directories(include ${OpenCV_INCLUDE_DIRS})


add_executable(Image-Classifier
        include/histogram.hpp
        src/histogram.cpp
        src/main.cpp)

target_link_libraries(Image-Classifier ${OpenCV_LIBS})

