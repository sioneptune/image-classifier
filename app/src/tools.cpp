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




void whitenImage(Mat& image) {
    Pixel* pixel;
    int limit = 25;

    for (int r = 0; r < image.rows; ++r) {
        for(int p = 0; p < image.cols; p++) {
            pixel = image.ptr<Pixel>(r, p);
            if( abs(pixel -> x - pixel -> y ) < limit && abs(pixel -> z - pixel -> y ) < limit && abs(pixel -> z - pixel -> x) < limit) {
                pixel->x = 255;
                pixel->y = 255;
                pixel->z = 255;
            }
        }
    }
}

void removeBorders(Mat& image) {
    Pixel* pixel;

    // top
    for (int r = 0; r < 10; ++r) {
        for(int p = 0; p < image.cols; p++) {
            pixel = image.ptr<Pixel>(r, p);
            pixel->x = 255;
            pixel->y = 255;
            pixel->z = 255;
        }
    }

    // bottom
    for (int r = image.rows - 1; r > image.rows - 11; --r) {
        for(int p = 0; p < image.cols; p++) {
            pixel = image.ptr<Pixel>(r, p);
            pixel->x = 255;
            pixel->y = 255;
            pixel->z = 255;
        }
    }

    // left
    for (int r = 0; r < 10; ++r) {
        for(int p = 0; p < image.rows; p++) {
            pixel = image.ptr<Pixel>(p, r);
            pixel->x = 255;
            pixel->y = 255;
            pixel->z = 255;
        }
    }

    // right
    for (int r = image.cols - 1; r > image.cols - 11; --r) {
        for(int p = 0; p < image.rows; p++) {
            pixel = image.ptr<Pixel>(p, r);
            pixel->x = 255;
            pixel->y = 255;
            pixel->z = 255;
        }
    }
}
