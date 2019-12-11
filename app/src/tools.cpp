#include "tools.hpp"

Mat regionOfInterest(const Mat& img, const Point2i& topLeft, const Point2i& bottomRight) {
    Rect roi = Rect(topLeft, bottomRight);
    return Mat(img, roi);
}

void saveImg(const string name, const Mat& img) {
//    vector<int> compression_params;
//    compression_params.push_back(IMWRITE_PNG_COMPRESSION);
//    compression_params.push_back(9);

    imwrite(name, img);
}