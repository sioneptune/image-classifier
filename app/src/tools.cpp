#include "tools.hpp"

Mat regionOfInterest(const Mat img, const int xTopLeft, const int yTopLeft, const int xBottomRight, const int yBottomRight) {
    int width = abs(xTopLeft - xBottomRight);
    int height = abs(yTopLeft - yBottomRight);
    Rect roi = Rect(xTopLeft, yTopLeft, width, height);
    return Mat(img, roi);
}

Mat regionOfInterest(const Mat img, const Point2i topLeft, const Point2i bottomRight) {
    return regionOfInterest(img, topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
}
