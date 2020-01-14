#include "pre-processing/SquareDetector.h"
#include "tools.h"

double SquareDetector::angle(const Point &pt1, const Point &pt2, const Point &pt0) {
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}


void SquareDetector::findSquares(const Mat &image, vector<Square> &squares, const int N) {
    squares.clear();

    Mat pyr, timg, gray0(image.size(), CV_8U), gray;

    // down-scale and upscale the image to filter out the noise
    pyrDown(image, pyr, Size(image.cols / 2, image.rows / 2));
    pyrUp(pyr, timg, image.size());

    // convert image to gray, no need to search squares on different color planes
    cvtColor(timg, gray0, COLOR_BGR2GRAY);

    vector<vector<Point>> contours;

    // try several threshold levels
    for (int l = 0; l < N; l++) {
        // hack: use Canny instead of zero threshold level.
        // Canny helps to catch squares with gradient shading
        if (l == 0) {
            // apply Canny. Take the upper threshold from slider and set the lower to 0 (forces edges merging)
            Canny(gray0, gray, 0, 50, 5);
            // dilate canny output to remove potential holes between edge segments
            dilate(gray, gray, Mat(), Point(-1, -1));
        } else {
            // apply threshold if l!=0:
            //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
            gray = gray0 >= (l + 1) * 255 / N;
        }

        // find contours and store them all as a list
        findContours(gray, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        vector<Point> approx;

        SquareDetector::selectContours(contours);

        // test each contour
        for (const auto &contour : contours) {
            // approximate contour with accuracy proportional to the contour perimeter
            approxPolyDP(contour, approx, arcLength(contour, true) * 0.02, true);

            // square contours should have 4 vertices after approximation
            // relatively large area (to filter out noisy contours) and be convex.
            // Note: absolute value of an area is used because
            // area may be positive or negative - in accordance with the contour orientation
            if (approx.size() == 4 &&
                fabs(contourArea(approx)) > 1000 &&
                isContourConvex(approx)) {
                double maxCosine = 0;

                for (int j = 2; j < 5; j++) {
                    // find the maximum cosine of the angle between joint edges
                    double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
                    maxCosine = MAX(maxCosine, cosine);
                }

                // if cosines of all angles are small (all angles are ~90 degree) then write quandrange
                // vertices to resultant sequence
                if (maxCosine < 0.3)
                    squares.push_back(approx);
            }
        }
    }
}

void SquareDetector::selectContours(vector<Square> &contours) {
    contours.erase(remove_if(
            contours.begin(), contours.end(),
            [](const auto c) {
                const double area = contourArea(c, false);
                return (area > 270 * 270) || (area < 250 * 250);
            }), contours.end());
}

void SquareDetector::extractTopLeftVertices(vector<Square> &squares, vector<Point> &topLefts) {
    for(auto &square: squares) {
        // Sort by x, and then compare the 2 vertices at the far end on the left (left edge points)
        sort(square.begin(), square.end(), sortByXComparator);
        // Add top left point: left edge but lowest y (bottom-left can have an x smaller than top-left after square detection)
        if(square[0].y < square[1].y) {
            topLefts.push_back(square[0]);
        } else {
            topLefts.push_back(square[1]);
        }
    }

    sort(topLefts.begin(), topLefts.end(), sortByXComparator);
}

void SquareDetector::drawSquares(Mat &image, const vector<Square> &squares, const string wndname) {
    RNG rng;
    for (auto &square : squares) {
        const Point *p = &square[0];
        int n = square.size();
        polylines(image, &p, &n, 1, true, Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), 3,
                  LINE_AA);
    }

    namedWindow(wndname, WINDOW_NORMAL);
    resizeWindow(wndname, 600, 600);
    imshow(wndname, image);
    waitKey();
}
