#include "tools.hpp"

Mat regionOfInterest(const Mat img, const Point2i topLeft, const Point2i bottomRight) {
    int width = abs(topLeft.x - bottomRight.x);
    int height = abs(topLeft.y - bottomRight.y);
    Rect roi = Rect(topLeft.x, topLeft.y, width, height);
    return Mat(img, roi);
}