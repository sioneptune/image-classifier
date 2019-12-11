//
// Created by zoe on 10/12/2019.
//

#ifndef IMAGE_CLASSIFIER_MATCH_PATTERN_H
#define IMAGE_CLASSIFIER_MATCH_PATTERN_H

#include <iostream>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

using namespace cv;

/**
 * Finds the more likely pattern to be present on the image among a list of patterns
 *
 * @param image scanned to find patterns in it
 * @param patternList list of the patterns to be tested
 * @param nbOfPatterns number of patterns in patternList
 * @param match_method method used in matchPattern
 * @return the index in the list of the best pattern found
 */
int findBestPattern(const Mat image, const Mat* patternList, const int nbOfPatterns, const int match_method);


/**
 * Returns the score of the match of the pattern on the image
 *
 * @param image scanned
 * @param pattern to be evaluated
 * @param match_method method used in matchTemplate (see openCV doc for details)
 * @return the score of the match between the image and the pattern
 */
double matchPattern(const Mat image, const Mat pattern, int const match_method);

#endif //IMAGE_CLASSIFIER_MATCH_PATTERN_H
