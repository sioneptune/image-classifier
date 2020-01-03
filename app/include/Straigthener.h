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
#define ROI_1_BR_Y 650

#define ROI_2_TL_X 0
#define ROI_2_TL_Y 3020
#define ROI_2_BR_X 670
#define ROI_2_BR_Y 3450

class Straigthener {

public:

    static void processImage(Mat &image);

    static void findTargets(Mat &image, vector<Point> &targets);

    static void display(Mat &image, vector<Point> &targets);


};


#endif //IMAGE_CLASSIFIER_STRAIGTHENER_H
