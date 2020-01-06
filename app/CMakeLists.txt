cmake_minimum_required(VERSION 3.14)
project(Image-Classifier)


set(CMAKE_CXX_STANDARD 14)
find_package(OpenCV REQUIRED)

include_directories(include ${OpenCV_INCLUDE_DIRS})


add_executable(Image-Classifier
        src/main.cpp
        src/SquareDetector.cpp include/SquareDetector.h
        src/tools.cpp include/tools.h
        src/Snippet.cpp include/Snippet.h
        src/Straightener.cpp include/Straightener.h
        src/MatchTemplate.cpp include/MatchTemplate.h
        src/main_test.cpp)

target_link_libraries(Image-Classifier ${OpenCV_LIBS})

