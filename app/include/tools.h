#ifndef IMAGE_CLASSIFIER_TOOLS_H
#define IMAGE_CLASSIFIER_TOOLS_H

#include <fstream>
#include <chrono>

using namespace std;

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"

using namespace cv;

#include "Snippet.h"

Mat regionOfInterest(const Mat& img, const Point2i& topLeft, const Point2i& bottomRight);

Mat regionOfInterest(const Mat& img, const Point2i& topLeft, int size);

Mat regionOfInterest(const Mat& img, const Point2i& topLeft, int topLeftOffset, int size);

Mat regionOfInterest(const Mat& img, int xTopLeft, int yTopLeft, int xBottomRight, int yBottomRight);


bool sortByXComparator(const Point& a, const Point& b);

void saveImg(const string name, const Mat& img);

void saveImgDesc(const string name, const Snippet& d);

Mat openImage(const String path);


class Timer
{
    std::chrono::steady_clock::time_point start;
    std::chrono::steady_clock::time_point end;
public:
    Timer() :start(std::chrono::steady_clock::now()) {};
    Timer& set_end() { end = std::chrono::steady_clock::now(); return *this; }
    void set_start() { start = std::chrono::steady_clock::now(); };
    std::string time_elapsed(const unsigned int divide = 1) const
    {
        const unsigned int s = std::max(divide, static_cast<unsigned int>(1));
        std::chrono::milliseconds total(std::chrono::duration_cast<std::chrono::seconds>(end - start));
        total /= s;
        std::chrono::hours   hh = std::chrono::duration_cast<std::chrono::hours>(total);
        total -= hh;
        std::chrono::minutes mm = std::chrono::duration_cast<std::chrono::minutes>(total);
        total -= mm;
        std::chrono::seconds ss = std::chrono::duration_cast<std::chrono::seconds>(total);
        total -= ss;
        std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(total);

        std::ostringstream out;
        if (hh > std::chrono::hours(0))
            out << hh.count() << "h ";
        if (mm > std::chrono::minutes(0))
            out << mm.count() << "m ";
        if (ss > std::chrono::seconds(0))
            out << ss.count() << "s";
        else
            out << ms.count() << "ms";
        return out.str();
    }
};

#endif //IMAGE_CLASSIFIER_TOOLS_H
