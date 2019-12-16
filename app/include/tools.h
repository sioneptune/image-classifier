#ifndef IMAGE_CLASSIFIER_TOOLS_H
#define IMAGE_CLASSIFIER_TOOLS_H

#include <fstream>

using namespace std;

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"

using namespace cv;

#include "Drawing.h"

Mat regionOfInterest(const Mat& img, const Point2i& topLeft, const Point2i& bottomRight);

Mat regionOfInterest(const Mat& img, const Point2i& topLeft, int size);

Mat regionOfInterest(const Mat& img, const Point2i& topLeft, int topLeftOffset, int size);

Mat regionOfInterest(const Mat& img, int xTopLeft, int yTopLeft, int xBottomRight, int yBottomRight);


void saveImg(const string name, const Mat& img);

void saveImgDesc(const string name, const Drawing& d);

Mat openImage(const String path);

#endif //IMAGE_CLASSIFIER_TOOLS_H
