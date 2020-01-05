#ifndef IMAGE_CLASSIFIER_SQUARE_DETECTOR_H
#define IMAGE_CLASSIFIER_SQUARE_DETECTOR_H

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>

using namespace cv;
using namespace std;

#define ROI_SIZE_SQUARE 248
#define ROI_TL_OFFSET 7

typedef vector<Point> Square;

// From: https://github.com/opencv/opencv/blob/master/samples/cpp/squares.cpp
class SquareDetector {

    /**
     * Helper function: finds cosine of angle between vectors pt0->pt1 and pt0->pt2
     *
     * @param pt1 end of first vector, starting at pt0
     * @param pt2 end of second vector, starting at pt0
     * @param pt0 origin of the two vectors
     * @return cosine of vectors pt0->pt1 and pt0->pt2
     */
    static double angle(const Point &pt1, const Point &pt2, const Point &pt0);

    /**
     * Removes some of the contours to keep only useful ones and reduce the number of operations done. Criteria based on
     * contour's area.
     */
    static void selectContours(vector<Square> &contours);

public:

    /**
     * Finds squares inside an image
     *
     * @param image in which we want to find squares
     * @param squares: filled with vectors of Point, representing vectors of the squares found during the process.
     *        Each element represents one square, with 4 vertices : top-left, top-right, bottom-right and bottom-left
     *        { [top-left, top-right, bottom-right, bottom-left] , ... }
     * @param N number of threshold levels to try in order to detect contours and squares. The bigger the more precise.
     *        Default = 11.
     */
    static void findSquares(const Mat &image, vector<Square> &squares, const int N = 11);

    /**
     * Draws squares on given image, and shows this image with highlighted squares
     *
     * @param image containing squares (described in @param squares), on which squares will be drawn.
     * @param squares in @param image
     * @param wndname: name given to the window that will display image with squares drawn on it. Default = "Squares Detected".
     */
    static void drawSquares(Mat &image, const vector<Square> &squares, const string wndname="Squares Detected");

    /**
     * Finds the top-left vertice in each given square, and sorts the vector by increasing x axis of the top-left Points
     *
     * @param squares in which we want to find only top-left Points
     * @param topLefts: vector that will be filled with top-left Points
     */
    static void extractTopLeftVertices(vector<Square> &squares, vector<Point> &topLefts);
};

#endif //IMAGE_CLASSIFIER_SQUARE_DETECTOR_H
