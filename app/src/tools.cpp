#include <iostream>
#include "tools.h"

Mat regionOfInterest(const Mat& img, const Point2i& topLeft, const Point2i& bottomRight) {
    Rect roi = Rect(topLeft, bottomRight);
    return Mat(img, roi);
}

Mat regionOfInterest(const Mat& img, const Point2i& topLeft, int topLeftOffset, int size) {
    const int tlox = topLeft.x + topLeftOffset;
    const int tloy = topLeft.y + topLeftOffset;
    return regionOfInterest(img, Point(tlox, tloy), Point(tlox + size, tloy + size));
}

Mat regionOfInterest(const Mat& img, const int xTopLeft, const int yTopLeft, const int xBottomRight, const int yBottomRight) {
    return regionOfInterest(img, Point(xTopLeft, yTopLeft), Point(xBottomRight, yBottomRight));
}


bool sortByXComparator(const Point& a, const Point& b) {
    return a.x < b.x;
}


void saveImg(const string name, const Mat& img) {
    imwrite(name, img);
}

void saveImgDesc(const string name, const Snippet& d) {
    ofstream file (name);
    if (file.is_open())
    {
        file << d;
        file.close();
    }
    else cerr << "Unable to open file: " << name << endl;
}

Mat openImage(const String path) {
    Mat image = imread(path);
    if (image.data == nullptr) {
        cerr << "Image not found: " << path << endl;
        waitKey(0);
        exit(EXIT_FAILURE);
    }
    return image;
}
