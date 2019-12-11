#include "square_detector.hpp"
#include "tools.hpp"

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
        findContours(gray, contours, RETR_CCOMP, CHAIN_APPROX_SIMPLE);

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
                return (area > 250 * 250) || (area < 230 * 230);
            }), contours.end());
}

bool sortXSup(const Point &a, const Point &b) {
    return a.x > b.x;
}

bool sortXInf(const Point &a, const Point &b) {
    return a.x < b.x;
}

void SquareDetector::selectSquares(vector<Square> &squares, vector<Point> &topLefts) {
    // TODO: decide if we keep it.
    // keep only squares with the right width
//    squares.erase(remove_if(
//            squares.begin(), squares.end(),
//            [](const auto square) {
//                const int width = abs(square[0].x - square[2].x);
//                return (width > 240) || (width < 230);
//            }), squares.end());

    vector<Point> allTopLefts;
    extractTopLeftVertices(squares, allTopLefts);

    // sort top-left Points by x
    sort(allTopLefts.begin(), allTopLefts.end(), sortXInf);

    // group topLefts close from each other: 5 lists of topLefts near each other
    vector<vector<Point>> closeTL = vector<vector<Point>>(5);
    Point previousTopLeft = allTopLefts[0];
    int groupCount = 0;

    for(Point &tl : allTopLefts) {
        // check if current square isn't near previous one
        if (tl.x > (previousTopLeft.x + SIZE_CHANGING_SQUARE)) {
            // Current square is at more than SIZE_CHANGING_SQUARE, it means it belongs to the next group
            groupCount++;
        }
        closeTL[groupCount].push_back(tl);
        previousTopLeft = tl;
    }

    for(auto group: closeTL) {
        // Keep only inner top-left point for a given square
        sort(group.begin(), group.end(), sortXSup);
        topLefts.push_back(group[0]);
    }
}

void SquareDetector::extractTopLeftVertices(vector<Square> &squares, vector<Point> &tlp) {
    for(auto &square: squares) {
        // Sort by x, and then compare the 2 vertices at the far end on the left (left edge points)
        sort(square.begin(), square.end(), [](const Point &a, const Point &b) { return a.x < b.x; });
        // Add top left point: left edge but lowest y (bottom-left can have an x smaller than top-left after square detection)
        if(square[0].y < square[1].y) {
            tlp.push_back(square[0]);
        } else {
            tlp.push_back(square[1]);
        }
    }
}

void SquareDetector::drawSquares(Mat &image, const vector<Square> &squares, const string wndname) {
    RNG rng;
    for (auto &square : squares) {
        const Point *p = &square[0];
        int n = square.size();
        //polylines(image, &p, &n, 1, true, Scalar(0, 255, 0), 3, LINE_AA);
        polylines(image, &p, &n, 1, true, Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), 3,
                  LINE_AA);
    }

    namedWindow(wndname, WINDOW_NORMAL);
    resizeWindow(wndname, 600, 600);
    imshow(wndname, image);
}



int _main(int argc, char **argv) {
    //static const char *names[] = {"../../../data/00000.png", "../../../data/00000_rotate.png", nullptr};
    static const char *names[] = {"../../../data/00000_row.png", nullptr};

    if (argc > 1) {
        names[0] = argv[1];
        names[1] = "0";
    }

    vector<Square> squares;

    for (int i = 0; names[i] != nullptr; i++) {
        string filename = names[i];
        Mat image = imread(filename, IMREAD_COLOR);
        if (image.empty()) {
            cout << "Couldn't load " << filename << endl;
            continue;
        }

        SquareDetector::findSquares(image, squares);
        vector<Point> topLefts;
        SquareDetector::selectSquares(squares, topLefts);

        //imshow("One square", regionOfInterest(image, squares[99][0], squares[99][2]));
        //SquareDetector::drawSquares(image, squares);

        for (int j = 0; j < topLefts.size(); j++) {
            saveImg("../../../data/icons/icon" + to_string(j) + ".png", regionOfInterest(image, topLefts[j], SIZE_ROI_SQUARE) );
        }

        int c = waitKey();
        if (c == 27)
            break;
    }

    return 0;
}