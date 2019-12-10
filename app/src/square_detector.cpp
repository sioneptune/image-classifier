#include "square_detector.hpp"

const char *wndname = "Square Detection Demo";

double SquareDetector::angle(const Point &pt1, const Point &pt2, const Point &pt0) {
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}


void SquareDetector::findSquares(const Mat &image, vector<vector<Point>> &squares, const int N) {
    squares.clear();

    Mat pyr, timg, gray0(image.size(), CV_8U), gray;

    // down-scale and upscale the image to filter out the noise
    pyrDown(image, pyr, Size(image.cols / 2, image.rows / 2));
    pyrUp(pyr, timg, image.size());
    vector<vector<Point> > contours;

    // find squares in every color plane of the image
    for (int c = 0; c < 3; c++) {
        int ch[] = {c, 0};
        mixChannels(&timg, 1, &gray0, 1, ch, 1);

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
            findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

            vector<Point> approx;

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
}


void SquareDetector::drawSquares(Mat &image, const vector<vector<Point>> &squares) {
    for (const auto &square : squares) {
        const Point *p = &square[0];
        int n = square.size();
        polylines(image, &p, &n, 1, true, Scalar(0, 255, 0), 3, LINE_AA);
    }

    namedWindow(wndname, WINDOW_NORMAL);
    resizeWindow(wndname, 600, 600);
    imshow(wndname, image);
}


int _main(int argc, char **argv) {
    static const char *names[] = {"../../../data/00000.png", "../../../data/00000_rotate.png", nullptr};
    SquareDetector sd;

    if (argc > 1) {
        names[0] = argv[1];
        names[1] = "0";
    }

    vector<vector<Point> > squares;

    for (int i = 0; names[i] != nullptr; i++) {
        string filename = names[i];
        Mat image = imread(filename, IMREAD_COLOR);
        if (image.empty()) {
            cout << "Couldn't load " << filename << endl;
            continue;
        }

        sd.findSquares(image, squares);
        sd.drawSquares(image, squares);

        int c = waitKey();
        if (c == 27)
            break;
    }

    return 0;
}