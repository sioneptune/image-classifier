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

enum FeatureFunction { FUNCTION_BOOL, FUNCTION_STRING, FUNCTION_INT, FUNCTION_DOUBLE, NB_CIRCLES };

class Feature {
public:
    const FeatureFunction name;
    explicit Feature(FeatureFunction f): name(f) {};
    virtual string getDescriptor() = 0;
    virtual string getValue() = 0;
    string getName() {
        switch (name) {
            case FUNCTION_BOOL: return "function_bool_or_any_name";
            case FUNCTION_STRING: return "function_something";
            case FUNCTION_INT: return "function_that_happened_to_be_useful";
            case FUNCTION_DOUBLE: return "function_double_trouble";
            case NB_CIRCLES: return "nb_of_circles";
        }
    }
};

class FeatureExtractor {
private:
    vector<Feature*> results;
public:
    FeatureExtractor();
    Feature* functionBool(Mat& img);
    Feature* functionString(Mat& img);
    Feature* functionInt(Mat& img);
    Feature* functionDouble(Mat& img);
    Feature* nbCircles(const Mat& img);

    void exportARFF(const vector<FeatureFunction>& list, string inputPath, string outputPath);
};

// Inherited Features

class FeatureBool : public Feature {
public:
    const string nominalValues = "{true, false}";
    bool value;
    FeatureBool(FeatureFunction f, bool val): Feature(f), value(val) {};
    string getDescriptor() override {   return "@ATTRIBUTE "+ getName() + " " + nominalValues;    }
    string getValue() override {    return to_string(value);    }
};

class FeatureString : public Feature {
public:
    const string nominalValues; // Should be in the format "{val_1, val_2, etc.}"
    string value;
    FeatureString(FeatureFunction f, string nominalVal, string val = ""): Feature(f), nominalValues(std::move(nominalVal)), value(std::move(val)) {};
    string getDescriptor() override {   return "@ATTRIBUTE "+ getName() + " " + nominalValues;    }
    string getValue() override {    return value;    }
};

class FeatureInt : public Feature {
public:
    int value;
    FeatureInt(FeatureFunction f, int val): Feature(f), value(val) {};
    string getDescriptor() override {   return "@ATTRIBUTE "+ getName() + " NUMERIC";    }
    string getValue() override {    return to_string(value);    }
};

class FeatureDouble : public Feature {
public:
    double value;
    FeatureDouble(FeatureFunction f, double val): Feature(f), value(val) {};
    string getDescriptor() override {   return "@ATTRIBUTE "+ getName()+ " NUMERIC";    }
    string getValue() override {    return to_string(value);    }
};

#endif //IMAGE_CLASSIFIER_FEATUREEXTRACTOR_H
