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

enum FeatureFunction { BARYCENTER, HEIGHT_WIDTH_RATIO };

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
    Mat image;
    Mat bbImage;
    Point upLeftCorner;
    Point downRightCorner;

    void setImage(const Mat& img) { image = img; }
    void setBBImage(const Mat& img) { bbImage = img; }

    vector<Feature *> barycenter() const;
    Feature* heightWidthRatio() const;


public:
    ~FeatureExtractor() {
        for(auto f: results) {
            delete f;
        }
    }
    void exportARFF(const vector<FeatureFunction>& list, const string inputPath, const string outputPath);
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
