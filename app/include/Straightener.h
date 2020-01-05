//
// Created by theotime on 03/01/2020.
//

#ifndef IMAGE_CLASSIFIER_STRAIGTHENER_H
#define IMAGE_CLASSIFIER_STRAIGTHENER_H

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>

using namespace cv;
using namespace std;

#define ROI_1_TL_X 1800
#define ROI_1_TL_Y 220
#define ROI_1_BR_X 2470
#define ROI_1_BR_Y 560

#define ROI_2_TL_X 50
#define ROI_2_TL_Y 3100
#define ROI_2_BR_X 670
#define ROI_2_BR_Y 3400

#define CROSS1X 2139.0
#define CROSS1Y 541.0
#define CROSS2X 298.0
#define CROSS2Y 3166.0

#define ROT_THRESHOLD 0.075

class Straightener {

public:

    static void processImage(Mat &image);

    static void findTargets(Mat &image, vector<Point> &targets);

    static void display(Mat &image, vector<Point> &targets);

    static void straighten(Mat &image);


};

struct CrossNotDetected : public std::exception
{
    const char * what () const throw ()
    {
        return "Less than two crosses were detected.";
    }
};

#endif //IMAGE_CLASSIFIER_STRAIGTHENER_H
