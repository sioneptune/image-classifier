#ifndef IMAGE_CLASSIFIER_TOOLS_HPP
#define IMAGE_CLASSIFIER_TOOLS_HPP

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

using namespace cv;

Mat regionOfInterest(const Mat img, const int xTopLeft, const int yTopLeft, const int xBottomRight, const int yBottomRight);

Mat regionOfInterest(const Mat img, const Point2i topLeft, const Point2i bottomRight);

#endif //IMAGE_CLASSIFIER_TOOLS_HPP
