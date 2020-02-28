cmake_minimum_required(VERSION 3.14)
project(Image-Classifier)

set(CMAKE_CXX_STANDARD 14)
find_package(OpenCV REQUIRED)

include_directories(include ${OpenCV_INCLUDE_DIRS})


add_executable(Image-Classifier
        src/pre-processing/main.cpp
        src/pre-processing/SquareDetector.cpp include/pre-processing/SquareDetector.h
        src/tools.cpp include/tools.h
        src/pre-processing/Snippet.cpp include/pre-processing/Snippet.h
        src/pre-processing/Rectificator.cpp include/pre-processing/Rectificator.h
        src/pre-processing/MatchTemplate.cpp include/pre-processing/MatchTemplate.h
        src/pre-processing/main_test.cpp src/feature-extraction/FeatureExtractor.cpp include/feature-extraction/FeatureExtractor.h)

target_link_libraries(Image-Classifier ${OpenCV_LIBS})

