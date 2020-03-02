#ifndef IMAGE_CLASSIFIER_FEATUREEXTRACTOR_H
#define IMAGE_CLASSIFIER_FEATUREEXTRACTOR_H

#include <iostream>
#include <utility>
#include <fstream>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "tools.h"

#define MAIN_LINES_THRESHOLD 100
#define ZONE_LINES_THRESHOLD 35

using namespace cv;
using namespace std;

// The main uses this enum to chose which features compute
enum FeatureFunction { BARYCENTER, CONVEX_HULL_AREA, HEIGHT_WIDTH_RATIO, HU_MOMENTS, LEVELS_OF_HIERARCHY, LINES, NUMBER_OF_ELEMENTS, PEAKS, PIXEL_RATE, ZONING_BARYCENTER, ZONING_HU_MOMENTS, ZONING_LINES, ZONING_PEAKS, ZONING_PIXEL_RATE };

/**
 * This class represents the features we extract. They are used in FeatureExtractor::exportARFF to generate the ARFF file.
 * There are below inherited classes for each kind of feature (numeric -double-, symbolic -string-).
 */
class Feature {
    const string name;
public:
    explicit Feature(string n): name(std::move(n)) {};

    /**
     * Generates the description of the feature ('@ATTRIBUTE name (Values | NUMERIC)') for ARFF file
     * @return a string representing the description
     */
    virtual string getDescriptor() = 0;
    virtual string getValue() = 0;
    string getName() {
        return name;
    }
};


class FeatureExtractor {
private:
    vector<Feature*> results; // stores all the features before writing the ARFF file
    Point upLeftCorner; // bounding box up left point (stored because used in several features)
    Point downRightCorner; // bounding box down right point (stored because used in several features)

    /**
     * Prepares the normalized image to call findContours and calls it
     * @param normImage to be analysed
     * @param contours of the image
     * @param hierarchy of the image elements
    */
    void getContours(const Mat &normImage, vector<vector<Point>>& contours, vector<Vec4i>& hierarchy) const;

    /**
     * Pastes the drawing bounding box on a defined bigger image so that all the images finally has the same height and width.
     * The image is enlarged according to its biggest dimension and centered according to the other.
     * @param bbImage is the drawing bounding box
     * @param size is the size of the final image
     * @return a normalized image (Mat)
     */
    Mat normalization(const Mat& bbImage, const int size) const;

    /**
     * Cuts the image according to decoupX and decoupY vectors
     * @param image to be cut
     * @param decoupX is a vector containing the cutting proportion along the X axis
     * @param decoupY is a vector containing the cutting proportion along the Y axis
     * @return a vector of the cut sub-images
     */
    vector<Mat> zones(Mat &image, vector<int> decoupX, vector<int> decoupY) const;

    /**
     * All the following methods are based on the same principle. From an image, they compute a set of features (may be 1) and return it.
     * @param normImage/image the image to be analysed
     * @param prefix is the prefix of the finale feature(s)'s name (used for zoning, for example)
     * @return a vector of the computed features or the feature if there's only one
     */
    // Computes the drawing barycenter
    vector<Feature *> barycenter(const Mat& normImage, const string prefix = "") const;
    // Computes the drawing convex hull area
    Feature* convexHullArea(const Mat& normImage, const string prefix = "") const;
    // Computes the drawing bounding box 'height / width' ratio
    Feature* heightWidthRatio(const string prefix = "") const;
    // Computes the drawing Hu's moments
    vector<Feature *> HuMoments(const Mat& normImage, const string prefix = "") const;
    // Computes the hierarchy levels maximum of the drawing elements
    Feature* levelsOfHierarchy(const Mat& image, const string prefix = "") const;
    // Computes the drawing number of straight lines
    Feature * lines(const Mat &normImage, const int threshNum, const string prefix = "") const;
    // Computes the drawing number of distinct elements
    Feature* numberOfElements(const Mat &image) const;
    // Computes the number of peaks of the drawing histograms from the X and Y axes
    vector<Feature *> peaks(const Mat &image, string prefix="") const;
    // Computes the drawing bounding box percentage of black pixels (pixels which belongs to the drawing)
    Feature* pixelRate(const Mat& normImage, const string prefix = "") const;

    // Particular feature which returns the final class of the image ('accident', for example) from the path of the image ('./accident/accident_000_00_1_1.png', for example)
    Feature* getClass(const string name) const;

    /**
     * For every image in zoneImages, calls the appropriate feature function (above) according to the value f
     * @param zoneImages represents the images to be analysed
     * @param f is the FeatureFunction we want to compute for every image in zoneImages
     * @return a vector of features containing the result of the feature function corresponding to f for every image in zoneImages
     */
    vector<Feature *> zoning_feature(const vector<Mat> zoneImages, const FeatureFunction f) const;


public:
    ~FeatureExtractor() {
        for(auto f: results) {
            delete f;
        }
    }

    /**
     * Generates an ARFF file
     * @param list of the feature functions to call
     * @param inputPath is the path to 'files_output.txt' file which contains the name of every image to be analysed
     * @param outputPath is the path where to write the final 'extracted_images.arff' file
     */
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
