//
// Created by theotime on 03/01/2020.
//

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
        Mat cross = getStructuringElement(MORPH_CROSS, Size(4, 4));
        erode(regions[i], regions[i], cross);

        // Eroding through a circle leaves only the round center
        Mat circle = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
        erode(regions[i], regions[i], circle);

        //finds all the white points (aka the center of the cross) and take their average
        vector<Point> whitePoints;
        findNonZero(regions[i], whitePoints);

        Point sum = Point(0, 0);
        for (int j = 0; j < whitePoints.size(); j++) {
            sum = sum + whitePoints[j];
        }
        Point average;
        if (i == 0) {
            average = Point((sum.x / whitePoints.size()) + ROI_1_TL_X,
                            (sum.y / whitePoints.size()) + ROI_1_TL_Y);
        } else {
            average = Point((sum.x / whitePoints.size()) + ROI_2_TL_X,
                            (sum.y / whitePoints.size()) + ROI_2_TL_Y);
        }

        targets.push_back(average);

        //display the ROI (to use only for debug)
//        namedWindow(wName, WINDOW_NORMAL);
//        resizeWindow(wName, 600,600);
//        imshow(wName, regions[i]);
//        int c = waitKey();

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
    //get the two crosshair target positions
    Straightener::processImage(image);
    Straightener::findTargets(image, targets);

    cout << targets << endl;

    //coordinates where we want the crosshairs to end up at
    double goal1_x = 2139;
    double goal1_y = 541;
    double goal2_x = 298;
    double goal2_y = 3166;

    //positions of our found targets
    double x0 = targets[0].x;
    double y0 = targets[0].y;
    double x1 = targets[1].x;
    double y1 = targets[1].y;

    double wantedAngle = atan((goal2_y - goal1_y) / (goal2_x - goal1_x)) * 180 / 3.1415952535897;
    double currentAngle = atan((y1 - y0) / (x1 - x0)) * 180 / 3.1415926535897;

    Mat transformation = getRotationMatrix2D(Point(image.cols / 2, image.rows / 2), currentAngle - wantedAngle, 1);

    cout << transformation << endl;
    warpAffine(originalImage, originalImage, transformation, image.size());

}

int _smain() {
    static const char *names[] = {"../../../data/00000.png", "../../../data/00000_rotate.png",
                                  "../../../data/00000_straightened.png", nullptr};

    Mat image;
    vector<Point> targets;

    for (int i = 0; names[i] != nullptr; i++) {

        targets = {};

        image = imread(names[i], IMREAD_COLOR);
        cout << "image size " << image.size << endl;
        if (image.empty()) {
            cout << "Couldn't load " << names[i] << endl;
            exit(0);
        }

//        Straightener::processImage(image);
//        cout << "image size " << image.size << endl;
//
//        Straightener::findTargets(image,targets);
//
//        cout << targets << endl;
//
//        Straightener::display(image, targets);

        Straightener::straighten(image);
        Straightener::display(image, targets);
        int c = waitKey();
    }
}