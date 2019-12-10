#ifndef IMAGE_CLASSIFIER_SQUARE_DETECTOR_HPP
#define IMAGE_CLASSIFIER_SQUARE_DETECTOR_HPP

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>

using namespace cv;
using namespace std;

class SquareDetector {

    /**
     * Helper function: finds cosine of angle between vectors pt0->pt1 and pt0->pt2
     *
     * @param pt1 end of first vector, starting at pt0
     * @param pt2 end of second vector, starting at pt0
     * @param pt0 origin of the two vectors
     * @return cosine of vectors pt0->pt1 and pt0->pt2
     */
    static double angle(const Point& pt1, const Point& pt2, const Point& pt0);

    public:
        static void findSquares(const Mat &image, vector<vector<Point>> &squares, const int thresh = 50, const int N =11);

        static void drawSquares(Mat &image, const vector<vector<Point>> &squares);

};
#endif //IMAGE_CLASSIFIER_SQUARE_DETECTOR_HPP
