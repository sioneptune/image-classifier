#ifndef IMAGE_CLASSIFIER_TOOLS_H
#define IMAGE_CLASSIFIER_TOOLS_H

#include <fstream>
#include <chrono>

using namespace std;

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"

using namespace cv;

#include "Snippet.h"

Mat regionOfInterest(const Mat& img, const Point2i& topLeft, const Point2i& bottomRight);

Mat regionOfInterest(const Mat& img, const Point2i& topLeft, int topLeftOffset, int size);

Mat regionOfInterest(const Mat& img, int xTopLeft, int yTopLeft, int xBottomRight, int yBottomRight);

bool sortByXComparator(const Point& a, const Point& b);

void saveImg(const string name, const Mat& img);

void saveImgDesc(const string name, const Snippet& d);

Mat openImage(const String path);

/** These 2 functions were developed to clean the final snippets but we were not sure if we could use it,
because the project specifies we are not allowed to change the images' colors. Indeed, these functions modify the images' background.
We decided not to use but to keep them if you want to check the code. */
typedef cv::Point3_<uint8_t> Pixel;

// Additional function to remove grey spots snippet
void whitenImage(Mat& image);

// Additional function to remove black borders of snippet
void removeBorders(Mat& image);


#endif //IMAGE_CLASSIFIER_TOOLS_H
