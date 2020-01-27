#ifndef IMAGE_CLASSIFIER_TOOLS_H
#define IMAGE_CLASSIFIER_TOOLS_H

#include <fstream>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "pre-processing/Snippet.h"

using namespace std;
using namespace cv;

Mat regionOfInterest(const Mat& img, const Point2i& topLeft, const Point2i& bottomRight);

Mat regionOfInterest(const Mat& img, const Point2i& topLeft, int topLeftOffset, int size);

Mat regionOfInterest(const Mat& img, int xTopLeft, int yTopLeft, int xBottomRight, int yBottomRight);

bool sortByXComparator(const Point& a, const Point& b);

void saveImg(const string name, const Mat& img);

void saveImgDesc(const string name, const Snippet& d);

Mat openImage(const String path);

/**
 * Gives back two points that define the bounding box around the drawing
 * @param image
 * @return two points, the top left (smallest x, smallest y) and the bottom right (biggest x, biggest y)
 */
vector<Point> boundingBox(const Mat image);

/**
 * Transforms background "white" pixels into real white pixels on the image
 * @param image
 * @return the final transformed image
 */
Mat whitenImage(const Mat& image);

/**
 * Computes the image average color without taking into account white pixels
 * @param image in BRG mode (or RGB mode)
 * @return a 3-component scalar, each representing the average of a channel (B ; G ; R or R ; G ; B depending on the image mode)
 */
Scalar getAverageColor(const Mat& image);

/**
 * Whitens all pixels which are not similar to the average color (see getAverageColor)
 * @param image
 * @return the final transformed image
 */
Mat removeNoise(const Mat& image);

#endif //IMAGE_CLASSIFIER_TOOLS_H
