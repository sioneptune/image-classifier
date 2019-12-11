//
// Created by zoe on 10/12/2019.
//

#include "match_pattern.h"
#include <iostream>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "tools.hpp"

using namespace cv;
using namespace std;


int main() {
    int match_methods[5] = {CV_TM_SQDIFF, CV_TM_SQDIFF_NORMED, CV_TM_CCORR, CV_TM_CCORR_NORMED, CV_TM_CCOEFF};

    Mat im = openImage("../models/00000.png");
    Mat bombIm = openImage("../models/bomb_bande.png");
    Mat casIm = openImage("../models/casualty_bande.png");
    Mat fbIm = openImage("../models/fire_brigade_bande.png");
    Mat floodIm = openImage("../models/flood_medium_bande.png");
    Mat polIm = openImage("../models/police_bande.png");

    Mat accident = openImage("../models/Accident.png");
    Mat bomb = openImage("../models/Bomb.png");
    Mat car = openImage("../models/Car.png");
    Mat casualty = openImage("../models/Casualty.png");
    Mat electricity = openImage("../models/Electricity.png");
    Mat fire = openImage("../models/Fire.png");
    Mat fire_brigade = openImage("../models/Fire_brigade.png");
    Mat flood = openImage("../models/Flood.png");
    Mat gas = openImage("../models/Gas.png");
    Mat injury = openImage("../models/Injury.png");
    Mat paramedics = openImage("../models/Paramedics.png");
    Mat person = openImage("../models/Person.png");
    Mat police = openImage("../models/Police.png");
    Mat road_block = openImage("../models/Road_block.png");

    Mat large = openImage("../models/large.png");
    Mat medium = openImage("../models/medium.png");
    Mat small = openImage("../models/small.png");

    Mat cat = openImage("../models/chat.jpg");


    Mat listOfSymbols[14] = {accident, bomb, car, casualty, electricity, fire, fire_brigade, flood, gas, injury, paramedics, person, police, road_block};
    Mat listOfText[3] = {small, medium, large};
    int result = findBestPattern(floodIm, listOfText, 3, match_methods[4]);
    imshow("Best pattern", listOfText[result]);


    waitKey(0);
    return EXIT_SUCCESS;

}

int findBestPattern(const Mat image, const Mat* patternList, const int nbOfPatterns, const int match_method) {
    int result = 0;
    double bestEval = matchPattern(image, patternList[0], match_method);
    double tmp;

    for (int i = 1; i < nbOfPatterns ; i++) {
        tmp = matchPattern(image, patternList[i], match_method);
        if (match_method == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED) {
            if (tmp < bestEval) {
                bestEval = tmp;
                result = i;
            }
        } else {
            if (tmp > bestEval) {
                bestEval = tmp;
                result = i;
            }
        }
    }
    return result;
}

double matchPattern(const Mat image, const Mat pattern, const int match_method) {
    // Initialize image of the matchTemplate result
    /// Doc of machTemplate(): "It returns a grayscale image, where each pixel denotes how much does the neighbourhood of that pixel match with template."
    Mat imMatch = Mat(Size(image.cols - pattern.cols + 1, image.rows - pattern.rows - 1), CV_32SC1);

    // Match pattern
    matchTemplate(image, pattern, imMatch, match_method);
    //normalize( imMatch, imMatch, 0, 1, NORM_MINMAX, -1, Mat() );

    // Localize the best match with minMaxLoc
    double minVal;
    double maxVal;
    Point minLoc;
    Point maxLoc;
    Point matchLoc;
    minMaxLoc(imMatch, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

    // Return the best evaluation of the match
    double eval;
    if (match_method == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED) {
        eval = minVal;
        matchLoc = minLoc;
    } else {
        eval = maxVal;
        matchLoc = maxLoc;
    }

//
//    // Draw rectangle where best pattern is detected
//    Mat result;
//    image.copyTo(result);
//    rectangle(result, matchLoc, Point(matchLoc.x + pattern.cols , matchLoc.y + pattern.rows ), Scalar::all(0), 2, 8, 0 );
//
//    // Show image
//    namedWindow("Original",WINDOW_NORMAL);
//    resizeWindow("Original", 600,600);
//    imshow( "Original", result);

    return eval;
}