#ifndef IMAGE_CLASSIFIER_FEATUREEXTRACTOR_H
#define IMAGE_CLASSIFIER_FEATUREEXTRACTOR_H

#include <iostream>
#include <utility>
#include <fstream>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "tools.h"

using namespace cv;
using namespace std;

enum FeatureFunction { BARYCENTER, HEIGHT_WIDTH_RATIO, PIXEL_RATE, LEVELS_OF_HIERARCHY, HU_MOMENTS, LINES, PEAKS, ZONING_BARYCENTER, ZONING_PIXEL_RATE, ZONING_HU_MOMENTS, ZONING_LINES, ZONING_PEAKS, NUMBER_OF_ELEMENTS };
class Feature {
    const string name;
public:
    explicit Feature(string n): name(std::move(n)) {};
    virtual string getDescriptor() = 0;
    virtual string getValue() = 0;
    string getName() {
        return name;
    }
};


class FeatureExtractor {
private:
    vector<Feature*> results;
    Point upLeftCorner;
    Point downRightCorner;

    Mat normalization(const Mat& bbImage, const int size) const;

    vector<Feature *> barycenter(const Mat& normImage, const string prefix = "") const;
    Feature* heightWidthRatio(const string prefix = "") const;
    Feature* pixelRate(const Mat& normImage, const string prefix = "") const;
    Feature* levelsOfHierarchy(const Mat& image, const string prefix = "") const;
    vector<Feature *> HuMoments(const Mat& normImage, const string prefix = "") const;
    vector<Mat> zones(Mat &image, vector<int> decoupX, vector<int> decoupY);
    Feature * lines(const Mat &normImage, const string prefix = "") const;
    vector<Feature *> zoning_feature(const vector<Mat> zoneImages, const FeatureFunction f) const;
    Feature* numberOfElements(const Mat &normImage) const;
    Feature* getClass(const string name) const;
    void getContours(const Mat &normImage, vector<vector<Point>>& contours, vector<Vec4i>& hierarchy) const;
    vector<Feature *> peaks(const Mat &img, string prefix="");


public:
    ~FeatureExtractor() {
        for(auto f: results) {
            delete f;
        }
    }
    void exportARFF(const vector<FeatureFunction>& list, const string& inputPath, const string& outputPath);

};



// Inherited Features
class FeatureBool : public Feature {
private:
    const string nominalValues = "{true, false}";
    bool value;
public:
    FeatureBool(string name, bool val): Feature(name), value(val) {};
    string getDescriptor() override {   return "@ATTRIBUTE "+ getName() + " " + nominalValues;    }
    string getValue() override {    return to_string(value);    }
};

class FeatureString : public Feature {
private:
    const string nominalValues; // Should be in the format "{val_1, val_2, etc.}"
    string value;
public:
    FeatureString(string name, string nominalVal, string val = ""): Feature(name), nominalValues(std::move(nominalVal)), value(std::move(val)) {};
    string getDescriptor() override {   return "@ATTRIBUTE "+ getName() + " " + nominalValues;    }
    string getValue() override {    return value;    }
};

class FeatureInt : public Feature {
private:
    int value;
public:
    FeatureInt(string name, int val): Feature(name), value(val) {};
    string getDescriptor() override {   return "@ATTRIBUTE "+ getName() + " NUMERIC";    }
    string getValue() override {    return to_string(value);    }
};

class FeatureDouble : public Feature {
private:
    double value;
public:
    FeatureDouble(string name, double val): Feature(name), value(val) {};
    string getDescriptor() override {   return "@ATTRIBUTE "+ getName() + " NUMERIC";    }
    string getValue() override {    return to_string(value);    }
};

#endif //IMAGE_CLASSIFIER_FEATUREEXTRACTOR_H
