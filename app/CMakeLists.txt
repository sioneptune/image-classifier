cmake_minimum_required(VERSION 3.14)
project(Image-Classifier)


set(CMAKE_CXX_STANDARD 14)
find_package(OpenCV REQUIRED)

include_directories(include ${OpenCV_INCLUDE_DIRS})


add_executable(Image-Classifier
        src/preprocessing/main.cpp
        src/preprocessing/SquareDetector.cpp include/preprocessing/SquareDetector.h
        src/tools.cpp include/tools.h
        src/preprocessing/Snippet.cpp include/preprocessing/Snippet.h
        src/preprocessing/Rectificator.cpp include/preprocessing/Rectificator.h
        src/preprocessing/MatchTemplate.cpp include/preprocessing/MatchTemplate.h
        src/preprocessing/main_test.cpp)

target_link_libraries(Image-Classifier ${OpenCV_LIBS})

