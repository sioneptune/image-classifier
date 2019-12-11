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


bool SquareDetector::sortN(const Point &a, const Point &b) {
    return atan2(a.x, b.y) < atan2(b.x, b.y);
}

bool sortInnerTopLeft(const Square &sa, const Square &sb) {
    return sa[1].x > sb[1].x;
}


void SquareDetector::selectSquares(vector<Square> &squares) {
    // keep only squares with the right width
//    squares.erase(remove_if(
//            squares.begin(), squares.end(),
//            [](const auto square) {
//                const int width = abs(square[0].x - square[2].x);
//                return (width > 240) || (width < 230);
//            }), squares.end());

    // sort points in each square to be sure to have the same order
    for (auto square: squares)
        sort(square.begin(), square.end(), sortN);

    // sort squares by top-left vertice
    sort(squares.begin(), squares.end(), [](auto sa, auto sb) { return sa[1].x < sb[1].x; } );

    // group squares close from each other: 5 lists of squares near each other
    vector<vector<Square>> closeSquares = vector<vector<Square>>(5);
    Point previousTopLeft = squares[0][1];
    int groupCount = 0;

    for(auto square : squares) {
        // check if current square isn't near previous one
        if (previousTopLeft.x < (square[1].x + SIZE_CHANGING_SQUARE)) {
            closeSquares[groupCount].push_back(square);
        } else {
            // Current square is at more than SIZE_CHANGING_SQUARE, it means it belongs to the next group
            groupCount++;
        }
    }

    for(auto group : closeSquares) {
        cout << group.size() << endl;
    }

    vector<Square> singleSquares;

    for(auto group: closeSquares) {
        sort(group.begin(), group.end(), sortInnerTopLeft);
        singleSquares.push_back(group[0]);
    }

    squares = singleSquares;
}


/// Function used only to draw squares in the right order
void sortClockwiseFromN(Square square) {
    swap(square[2], square[3]);
}

void SquareDetector::drawSquares(Mat &image, const vector<Square> &squares, const string wndname) {
    RNG rng;
    for (auto &square : squares) {
        sortClockwiseFromN(square);
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



int main(int argc, char **argv) {
    //static const char *names[] = {"../../../data/00000.png", "../../../data/00000_rotate.png", nullptr};
    static const char *names[] = {"../../../data/00000.png", nullptr};

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
        cout << squares.size() << endl;
        SquareDetector::selectSquares(squares);
        cout << squares.size() << endl;

        //imshow("One square", regionOfInterest(image, squares[99][0], squares[99][2]));
        SquareDetector::drawSquares(image, squares);

        for (int j = 0; j < squares.size(); j++) {
            saveImg("../../../data/icons/icon" + to_string(j) + ".png",
                    regionOfInterest(image, squares[j][0], squares[j][2]));
        }

        int c = waitKey();
        if (c == 27)
            break;
    }

    return 0;
}