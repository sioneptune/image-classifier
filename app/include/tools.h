#ifndef IMAGE_CLASSIFIER_TOOLS_H
#define IMAGE_CLASSIFIER_TOOLS_H

#include <fstream>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "preprocessing/Snippet.h"

using namespace std;
using namespace cv;

Mat regionOfInterest(const Mat& img, const Point2i& topLeft, const Point2i& bottomRight);

Mat regionOfInterest(const Mat& img, const Point2i& topLeft, int topLeftOffset, int size);

Mat regionOfInterest(const Mat& img, int xTopLeft, int yTopLeft, int xBottomRight, int yBottomRight);

bool sortByXComparator(const Point& a, const Point& b);

void saveImg(const string name, const Mat& img);

void saveImgDesc(const string name, const Snippet& d);

Mat openImage(const String path);

#endif //IMAGE_CLASSIFIER_TOOLS_H
