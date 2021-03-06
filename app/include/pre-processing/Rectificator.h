#ifndef IMAGE_CLASSIFIER_RECTIFICATOR_H
#define IMAGE_CLASSIFIER_RECTIFICATOR_H

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

#define CROSS1X 2205.0
#define CROSS1Y 472.0
#define CROSS2X 255.0
#define CROSS2Y 3235.0

#define ROT_THRESHOLD 0.075
#define SCALE_THRESHOLD 25
#define SHIFT_THRESHOLD 5

class Rectificator {

public:

    /**
     * Pre-processing function that denoises and image and converts it to grayscale
     *
     * @param image the image matrix we want to process
     */
    static void processImage(Mat &image);

    /**
     * Detects the two targets in the image
     *
     * @param image in which we search for the targets
     * @param targets : vector that is filled with the points the function detects as the center of each cross
     */
    static void findTargets(Mat &image, vector<Point> &targets);

    /**
     * Debug function. Displays the image with circles around where the targets have been detected
     *
     * @param image to display
     * @param targets we want to show on the image
     */
    static void display(Mat &image, vector<Point> &targets);

    /**
     * Takes an image, detects the crosses, and rotates it so the image is straight and scaled correctly, then shifts
     * it into correct place if required
     *
     * @param image to rectify
     */
    static void rectify(Mat &originalImage);


};

/**
 * Custom exception to be raised when one of the crosses is not detected in the straightening process
 */
struct CrossNotDetected : public std::exception
{
    const char * what () const throw ()
    {
        return "Less than two crosses were detected.";
    }
};

#endif //IMAGE_CLASSIFIER_RECTIFICATOR_H
