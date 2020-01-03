cmake_minimum_required(VERSION 3.14)
project(Image-Classifier)


set(CMAKE_CXX_STANDARD 14)
find_package(OpenCV REQUIRED)

include_directories(include ${OpenCV_INCLUDE_DIRS})


add_executable(Image-Classifier
        src/main.cpp
        src/histogram.cpp include/histogram.h
        src/square_detector.cpp include/square_detector.h
        src/tools.cpp include/tools.h
        src/Drawing.cpp include/Drawing.h
        src/match_pattern.cpp include/match_pattern.h src/Straigthener.cpp src/Straigthener.h)

target_link_libraries(Image-Classifier ${OpenCV_LIBS})

