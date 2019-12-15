#ifndef IMAGE_CLASSIFIER_TOOLS_HPP
#define IMAGE_CLASSIFIER_TOOLS_HPP

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"

using namespace cv;
using namespace std;

Mat regionOfInterest(const Mat& img, const Point2i& topLeft, const Point2i& bottomRight);

Mat regionOfInterest(const Mat& img, const Point2i& topLeft, int size);

Mat regionOfInterest(const Mat& img, const Point2i& topLeft, int topLeftOffset, int size);

void saveImg(const string name, const Mat& img);

Mat openImage(const String path);

#endif //IMAGE_CLASSIFIER_TOOLS_HPP
