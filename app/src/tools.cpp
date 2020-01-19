#include <iostream>
#include "tools.h"

Mat regionOfInterest(const Mat &img, const Point2i &topLeft, const Point2i &bottomRight) {
    Rect roi = Rect(topLeft, bottomRight);
    return Mat(img, roi);
}

Mat regionOfInterest(const Mat &img, const Point2i &topLeft, int topLeftOffset, int size) {
    const int tlox = topLeft.x + topLeftOffset;
    const int tloy = topLeft.y + topLeftOffset;
    return regionOfInterest(img, Point(tlox, tloy), Point(tlox + size, tloy + size));
}

Mat regionOfInterest(const Mat &img, const int xTopLeft, const int yTopLeft, const int xBottomRight,
                     const int yBottomRight) {
    return regionOfInterest(img, Point(xTopLeft, yTopLeft), Point(xBottomRight, yBottomRight));
}


bool sortByXComparator(const Point &a, const Point &b) {
    return a.x < b.x;
}


void saveImg(const string name, const Mat &img) {
    imwrite(name, img);
}

void saveImgDesc(const string name, const Snippet &d) {
    ofstream file(name);
    if (file.is_open()) {
        file << d;
        file.close();
    } else cerr << "Unable to open file: " << name << endl;
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

vector<Point> boundingBox(const Mat image) {
    vector<Point> nonzero;
    Mat binim;
    cvtColor(image, binim, COLOR_BGR2GRAY);
    threshold(binim, binim, 200, 255, THRESH_BINARY_INV);
    findNonZero(binim, nonzero);
    int top = 0;
    int bottom = INT_MAX;
    int left = INT_MAX;
    int right = 0;

    for (Point p : nonzero) {
        if (p.x > right) right = p.x;
        if (p.x < left) left = p.x;
        if (p.y > top) top = p.y;
        if (p.y < bottom) bottom = p.y;

    }

    return {Point(left, top), Point(right, bottom)};
}
