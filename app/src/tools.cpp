#include "tools.hpp"

Mat regionOfInterest(const Mat& img, int xTopLeft, int yTopLeft, int xBottomRight, int yBottomRight) {
    int width = abs(xTopLeft - xBottomRight);
    int height = abs(yTopLeft - yBottomRight);
    Rect roi = Rect(xTopLeft, yTopLeft, width, height);
    return Mat(img, roi);
}

Mat regionOfInterest(const Mat& img, const Point2i& topLeft, const Point2i& bottomRight) {
    return regionOfInterest(img, topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
}