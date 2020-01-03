#include "match_pattern.h"
#include "opencv2/imgproc.hpp"
#include "tools.h"

using namespace cv;

const Mat accident = openImage("../../data/patterns/Accident.png");
const Mat bomb = openImage("../../data/patterns/Bomb.png");
const Mat car = openImage("../../data/patterns/Car.png");
const Mat casualty = openImage("../../data/patterns/Casualty.png");
const Mat electricity = openImage("../../data/patterns/Electricity.png");
const Mat fire = openImage("../../data/patterns/Fire.png");
const Mat fire_brigade =  openImage("../../data/patterns/Fire_brigade.png");
const Mat flood = openImage("../../data/patterns/Flood.png");
const Mat gas = openImage("../../data/patterns/Gas.png");
const Mat injury =  openImage("../../data/patterns/Injury.png");
const Mat paramedics = openImage("../../data/patterns/Paramedics.png");
const Mat person = openImage("../../data/patterns/Person.png");
const Mat police  = openImage("../../data/patterns/Police.png");
const Mat road_block = openImage("../../data/patterns/Road_block.png");

const Mat large = openImage("../../data/patterns/large.png");
const Mat medium = openImage("../../data/patterns/medium.png");
const Mat small = openImage("../../data/patterns/small.png");


MatchPattern::MatchPattern() {
    listOfSymbols = {accident, bomb, car, casualty, electricity, fire, fire_brigade, flood, gas, injury, paramedics, person, police, road_block};
    listOfSizes = {small, medium, large};
    match_method = CV_TM_CCOEFF;
}

string MatchPattern::findSymbol(const Mat& image) const {
    int result = findBestPattern(image, listOfSymbols);
    switch(result) {
        case 0: return "accident";
        case 1: return "bomb";
        case 2: return "car";
        case 3: return "casualty";
        case 4: return "electricity";
        case 5: return "fire";
        case 6: return "fire brigade";
        case 7: return "flood";
        case 8: return "gas";
        case 9: return "injury";
        case 10: return "paramedics";
        case 11: return "person";
        case 12: return "police";
        case 13: return "road block";
        default: return "";
    }

}

string MatchPattern::findSize(const Mat& image) const{
    int result = findBestPattern(image, listOfSizes);
    switch(result) {
        case 0: return "small";
        case 1: return "medium";
        case 2: return "large";
        default: return "";
    }
}


int MatchPattern::findBestPattern(const Mat& image, const vector<Mat>& patternList) const {
    int result = 0;
    double bestEval = matchPattern(image, patternList[0], match_method);
    double tmp;

    for (int i = 1; i < patternList.size() ; i++) {
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

double MatchPattern::matchPattern(const Mat& image, const Mat& pattern, const int match_method) const {
    // Initialize image of the matchTemplate result
    /// Doc of matchTemplate(): "It returns a grayscale image, where each pixel denotes how much does the neighbourhood of that pixel match with template."
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

    return eval;
}