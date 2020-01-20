#include <iostream>
#include "tools.h"

typedef cv::Point3_<uint8_t> Pixel;


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

Mat whitenImage(const Mat& image) {
    Mat copy;
    image.copyTo(copy);
    Pixel* pixel;
    int limit = 20;

    for (int r = 0; r < copy.rows; ++r) {
        for(int c = 0; c < copy.cols; c++) {
            pixel = copy.ptr<Pixel>(r, c);

            // transforms grey pixels into white ones
            if( abs(pixel -> x - pixel -> y ) < limit && abs(pixel -> z - pixel -> y ) < limit && abs(pixel -> z - pixel -> x) < limit) {
                pixel->x = 255;
                pixel->y = 255;
                pixel->z = 255;
            }
        }
    }

    return copy;
}

Scalar getAverageColor(const Mat& image) {
    Mat whiteImg = whitenImage(image);

    // mean of BGR channels without taking into account white pixels
    int bMean = 0, gMean = 0, rMean = 0, nbOfPixels = 0;

    for (int r = 0; r < whiteImg.rows; ++r) {
        for (int c = 0; c < whiteImg.cols; c++) {
            Pixel* pixel = whiteImg.ptr<Pixel>(r, c);
            if( pixel -> x != 255 && pixel -> y != 255 && pixel -> z != 255) {
                nbOfPixels += 1;
                bMean += pixel -> x;
                gMean += pixel -> y;
                rMean += pixel -> z;
            }
        }
    }

    return Scalar(1.0*bMean/nbOfPixels, 1.0*gMean/nbOfPixels, 1.0*rMean/nbOfPixels);
}

Mat removeNoise(const Mat& image) {
    // initialization of variables
    Mat copy;
    image.copyTo(copy);
    Scalar averagePixel = getAverageColor(image);
    int limit = 50;
    double bMin = averagePixel[0] - limit;
    double bMax = averagePixel[0] + limit;
    double gMin = averagePixel[1] - limit;
    double gMax = averagePixel[1] + limit;
    double rMin = averagePixel[2] - limit;
    double rMax = averagePixel[2] + limit;

    // transformation of pixels which aren't not similar to the average color into white pixels
    for (int r = 0; r < copy.rows; ++r) {
        for (int c = 0; c < copy.cols; c++) {
            Pixel *pixel = copy.ptr<Pixel>(r, c);

            if( !((bMin <= pixel->x && pixel->x <= bMax) &&
                  (gMin <= pixel->y && pixel->y <= gMax) &&
                  (rMin <= pixel->z && pixel->z <= rMax)) ){
                pixel->x = 255;
                pixel->y = 255;
                pixel->z = 255;
            }
        }
    }

    // erosion to fill holes
    int size = 2;
    Mat element = getStructuringElement( MORPH_ELLIPSE,
                                         Size( 2*size + 1, 2*size+1 ),
                                         Point( size, size ) );

    cvtColor(copy, copy, COLOR_BGR2GRAY);
    erode(copy, copy, element);

    return copy;
}
