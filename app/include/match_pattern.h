#ifndef IMAGE_CLASSIFIER_MATCH_PATTERN_H
#define IMAGE_CLASSIFIER_MATCH_PATTERN_H

#include <iostream>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

using namespace cv;
using namespace std;

class MatchPattern {

public:
    MatchPattern();
    int match_method;

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
        * Finds the more likely pattern to be present on the image among a list of patterns
        *
        * @param image scanned to find patterns in it
        * @param patternList list of the patterns to be tested
        * @return the index in the list of the best pattern found
        */
        int findBestPattern(const Mat& image, const vector<Mat>& patternList) const;


        /**
         * Returns the score of the match of the pattern on the image
         *
         * @param image scanned
         * @param pattern to be evaluated
         * @param match_method method used in matchTemplate (see openCV doc for details)
         * @return the score of the match between the image and the pattern
         */
        double matchPattern(const Mat& image, const Mat& pattern) const;

};

#endif //IMAGE_CLASSIFIER_MATCH_PATTERN_H
