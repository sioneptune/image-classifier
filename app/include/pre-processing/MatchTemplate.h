#ifndef IMAGE_CLASSIFIER_MATCHTEMPLATE_H
#define IMAGE_CLASSIFIER_MATCHTEMPLATE_H

#include <iostream>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

using namespace cv;
using namespace std;

typedef cv::Point3_<uint8_t> Pixel;

class MatchTemplate {

    Mat accident, bomb, car, casualty, electricity, fire, fire_brigade, flood, gas, injury, paramedics, person, police, road_block;
    Mat large, medium, small;

public:
    MatchTemplate(const string templatePath);
    int match_method;

    bool hasSizeLabel(const Mat& image) const;

    /**
     * Finds the symbol on the image
     *
     * @param image to be analysed
     * @return a string with the name of the symbol on the image
     */
    string findSymbol(const Mat& image) const;

    /**
     * Finds the size written on the image
     *
     * @param image to be analysed
     * @return a string with the name of the size written on the image
     */
    string findSize(const Mat& image) const;



private:
        vector<Mat> listOfSymbols;
        vector<Mat> listOfSizes;

        /**
        * Finds the most likely template to be present on the image among a list of templates
        *
        * @param image scanned to find templates on it
        * @param templateList list of the templates to be tested
        * @return the index in the list of the best template found
        */
        int findBestTemplate(const Mat& image, const vector<Mat>& templateList) const;


        /**
         * Returns the score of the match of the template on the image
         *
         * @param image scanned
         * @param templateToTest template to be evaluated
         * @param match_method method used in matchTemplate (see openCV doc for details)
         * @return the score of the match between the image and the template
         */
        double evaluateTemplate(const Mat& image, const Mat& templateToTest) const;

};

#endif //IMAGE_CLASSIFIER_MATCHTEMPLATE_H
