#include <iostream>
#include "tools.h"

Mat regionOfInterest(const Mat& img, const Point2i& topLeft, const Point2i& bottomRight) {
    Rect roi = Rect(topLeft, bottomRight);
    return Mat(img, roi);
}

Mat regionOfInterest(const Mat& img, const Point2i& topLeft, int size) {
    return regionOfInterest(img, topLeft, Point(topLeft.x + size, topLeft.y + size));
}

Mat regionOfInterest(const Mat& img, const Point2i& topLeft, int topLeftOffset, int size) {
    const int tlox = topLeft.x + topLeftOffset;
    const int tloy = topLeft.y + topLeftOffset;
    return regionOfInterest(img, Point(tlox, tloy), Point(tlox + size, tloy + size));
}

void saveImg(const string name, const Mat& img) {
//    vector<int> compression_params;
//    compression_params.push_back(IMWRITE_PNG_COMPRESSION);
//    compression_params.push_back(9);

    imwrite(name, img);
}

void saveImgDesc(const string name, const Drawing& d) {
    ofstream file (name);
    if (file.is_open())
    {
        file << d;
        file.close();
    }
    else cerr << "Unable to open file";
}

Mat openImage(const String path) {
    Mat image = imread(path);
    if (image.data == nullptr) {
        std::cerr << "Image not found: " << path << std::endl;
        waitKey(0);
        //system("pause");
        exit(EXIT_FAILURE);
    }
    return image;
}
