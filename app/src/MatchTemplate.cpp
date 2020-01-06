#include "MatchTemplate.h"
#include "opencv2/imgproc.hpp"
#include "tools.h"

using namespace cv;


MatchTemplate::MatchTemplate(const string templatePath) {
    accident = openImage(templatePath + "Accident.png");
    bomb = openImage(templatePath + "Bomb.png");
    car = openImage(templatePath + "Car.png");
    casualty = openImage(templatePath + "Casualty.png");
    electricity = openImage(templatePath + "Electricity.png");
    fire = openImage(templatePath + "Fire.png");
    fire_brigade =  openImage(templatePath + "Fire_brigade.png");
    flood = openImage(templatePath + "Flood.png");
    gas = openImage(templatePath + "Gas.png");
    injury =  openImage(templatePath + "Injury.png");
    paramedics = openImage(templatePath + "Paramedics.png");
    person = openImage(templatePath + "Person.png");
    police  = openImage(templatePath + "Police.png");
    road_block = openImage(templatePath + "Road_block.png");

    large = openImage(templatePath + "large.png");
    medium = openImage(templatePath + "medium.png");
    small = openImage(templatePath + "small.png");
    listOfSymbols = {accident, bomb, car, casualty, electricity, fire, fire_brigade, flood, gas, injury, paramedics, person, police, road_block};
    listOfSizes = {small, medium, large};
    match_method = TM_CCOEFF;
}

string MatchTemplate::findSymbol(const Mat& image) const {
    int result = findBestTemplate(image, listOfSymbols);
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

string MatchTemplate::findSize(const Mat& image) const{
    int result = findBestTemplate(image, listOfSizes);
    switch(result) {
        case 0: return "small";
        case 1: return "medium";
        case 2: return "large";
        default: return "";
    }
}


int MatchTemplate::findBestTemplate(const Mat& image, const vector<Mat>& templateList) const {
    int result = 0;
    double bestEval = evaluateTemplate(image, templateList[0]);
    double tmp;

    for (int i = 1; i < templateList.size() ; i++) {
        tmp = evaluateTemplate(image, templateList[i]);
        if (match_method == TM_SQDIFF || match_method == TM_SQDIFF_NORMED) {
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

double MatchTemplate::evaluateTemplate(const Mat& image, const Mat& templateToTest) const {
    // Initialize image of the matchTemplate result
    /// Doc of matchTemplate(): "It returns a grayscale image, where each pixel denotes how much does the neighbourhood of that pixel match with template."
    Mat imMatch = Mat(Size(image.cols - templateToTest.cols + 1, image.rows - templateToTest.rows - 1), CV_32SC1);

    // Match templateToTest
    matchTemplate(image, templateToTest, imMatch, match_method);
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
    if (match_method == TM_SQDIFF || match_method == TM_SQDIFF_NORMED) {
        eval = minVal;
        matchLoc = minLoc;
    } else {
        eval = maxVal;
        matchLoc = maxLoc;
    }

    return eval;
}