#ifndef IMAGE_CLASSIFIER_SQUARE_DETECTION_HPP
#define IMAGE_CLASSIFIER_SQUARE_DETECTION_HPP

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>

using namespace cv;
using namespace std;

// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
static double angle( Point pt1, Point pt2, Point pt0 );

static void findSquares( const Mat& image, vector<vector<Point> >& squares );

static void drawSquares( Mat& image, const vector<vector<Point> >& squares );

#endif //IMAGE_CLASSIFIER_SQUARE_DETECTION_HPP
