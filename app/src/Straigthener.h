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

class Straigthener {

public:

    static void processImage(Mat &image);

    static void findTargets(Mat &image, vector<Point> &targets);

    static void display(Mat &image, vector<Point> &targets);


};


#endif //IMAGE_CLASSIFIER_STRAIGTHENER_H
