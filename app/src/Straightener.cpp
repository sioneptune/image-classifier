#include "Straightener.h"
#include "tools.h"

void Straightener::processImage(Mat &image) {
    Mat tiny, clean;

    pyrDown(image, tiny, Size(image.cols / 2, image.rows / 2));
    pyrUp(tiny, clean, image.size());
    cvtColor(clean, image, COLOR_BGR2GRAY);
}

void Straightener::findTargets(Mat &image, vector<Point> &targets) {
    Mat analyze;
    image.copyTo(analyze);
    Mat ro1 = regionOfInterest(analyze, Point(ROI_1_TL_X, ROI_1_TL_Y), Point(ROI_1_BR_X, ROI_1_BR_Y));
    Mat ro2 = regionOfInterest(analyze, Point(ROI_2_TL_X, ROI_2_TL_Y), Point(ROI_2_BR_X, ROI_2_BR_Y));
    vector<Mat> regions = {ro1, ro2};

    for (int i = 0; i < regions.size(); i++) {
        threshold(regions[i], regions[i], 200, 255, THRESH_BINARY_INV);
        string wName = to_string(i);

        // Eroding through a 4x4 cross, which will make the center bulky and the bars super thin
        Mat cross = getStructuringElement(MORPH_CROSS, Size(5, 5));
        erode(regions[i], regions[i], cross);

        // Eroding through a circle leaves only the round center
        Mat circle = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
        erode(regions[i], regions[i], circle);

        // Finds all the white points (aka the center of the cross) and take their average
        vector<Point> whitePoints;
        findNonZero(regions[i], whitePoints);

        Point sum = Point(0, 0);
        for (int j = 0; j < whitePoints.size(); j++) {
            sum = sum + whitePoints[j];
        }

        if (whitePoints.size() != 0) {
            Point average;
            if (i == 0) {
                average = Point((sum.x / whitePoints.size()) + ROI_1_TL_X,
                                (sum.y / whitePoints.size()) + ROI_1_TL_Y);
            } else {
                average = Point((sum.x / whitePoints.size()) + ROI_2_TL_X,
                                (sum.y / whitePoints.size()) + ROI_2_TL_Y);
            }

            targets.push_back(average);
        }
    }
}

void Straightener::display(Mat &image, vector<Point> &targets) {
    Mat disp;
    image.copyTo(disp);
    for (int i = 0; i < targets.size(); i++) {
        circle(disp, targets[i], 100, Scalar(0, 0, 255), 2);
    }
    const string wName = "Results";

    namedWindow(wName, WINDOW_NORMAL);
    resizeWindow(wName, 600, 600);
    imshow(wName, disp);
}

void Straightener::straighten(Mat &originalImage) {

    vector<Point> targets;
    Mat image;
    originalImage.copyTo(image);
    // Get the two crosshair target positions
    Straightener::processImage(image);

    Straightener::findTargets(image, targets);

    if(targets.size() != 2) throw CrossNotDetected();

    // Positions of our found targets
    double x0 = targets[0].x;
    double y0 = targets[0].y;
    double x1 = targets[1].x;
    double y1 = targets[1].y;

    double wantedAngle = atan((CROSS2Y - CROSS1Y) / (CROSS2X - CROSS1X)) * 180 / CV_PI;
    double currentAngle = atan((y1 - y0) / (x1 - x0)) * 180 / CV_PI;

    if (abs(wantedAngle - currentAngle) > ROT_THRESHOLD) {
        Mat transformation = getRotationMatrix2D(Point(image.cols / 2, image.rows / 2), currentAngle - wantedAngle,1);
        warpAffine(originalImage, originalImage, transformation, image.size());
    }
}
