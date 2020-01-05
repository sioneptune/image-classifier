//
// Created by theotime on 03/01/2020.
//

#include "Straightener.h"
#include "tools.h"

struct CrossNotDetected : public std::exception
{
    const char * what () const throw ()
    {
        return "Less than two crosses were detected";
    }
}

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

        //display the ROI (to use only for debug)
//        namedWindow(wName, WINDOW_NORMAL);
//        resizeWindow(wName, 600, 600);
//        imshow(wName, regions[i]);
//        int c = waitKey();

        // Eroding through a 4x4 cross, which will make the center bulky and the bars super thin
        Mat cross = getStructuringElement(MORPH_CROSS, Size(5, 5));
        erode(regions[i], regions[i], cross);

        //display the ROI (to use only for debug)
//        imshow(wName, regions[i]);
//        c = waitKey();

        // Eroding through a circle leaves only the round center
        Mat circle = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
        erode(regions[i], regions[i], circle);

        //display the ROI (to use only for debug)
//        imshow(wName, regions[i]);
//        c = waitKey();

        //finds all the white points (aka the center of the cross) and take their average
        vector<Point> whitePoints;
        findNonZero(regions[i], whitePoints);

        Point sum = Point(0, 0);
        for (int j = 0; j < whitePoints.size(); j++) {
            sum = sum + whitePoints[j];
        }

//        cout << "found " << whitePoints.size() << " white points" << endl;

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

        //display the ROI (to use only for debug)
//        imshow(wName, regions[i]);
//        c = waitKey();

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

//    cout << targets << endl;
    if(targets.size() != 2) throw CrossNotDetected();

    //positions of our found targets
    double x0 = targets[0].x;
    double y0 = targets[0].y;
    double x1 = targets[1].x;
    double y1 = targets[1].y;

    double wantedAngle = atan((CROSS2Y - CROSS1Y) / (CROSS2X - CROSS1X)) * 180 / CV_PI;
    double currentAngle = atan((y1 - y0) / (x1 - x0)) * 180 / CV_PI;
    // cout << abs(wantedAngle - currentAngle) << " degrees of difference" << endl;

    if (abs(wantedAngle - currentAngle) > ROT_THRESHOLD) {
        Mat transformation = getRotationMatrix2D(Point(image.cols / 2, image.rows / 2), currentAngle - wantedAngle,
                                                 1);

        //cout << transformation << endl;
        warpAffine(originalImage, originalImage, transformation, image.size());

    }
}

int eemain() {
    //static const char *names[] = {"../../../data/00000.png", "../../../data/00000_rotate.png",
    // "../../../data/00000_straightened.png", nullptr};
    static const char *names[] = {
            "../../data/database/00000.png",//"../../data/database/00001.png","../../data/database/00002.png","../../data/database/00003.png","../../data/database/00004.png",nullptr};
            "../../data/database/00715.png",
            "../../data/database/01316.png",
            "../../data/database/01712.png",
            "../../data/database/02608.png",
            "../../data/database/03200.png",
            "../../data/database/03205.png",
            "../../data/database/03311.png",
            "../../data/database/03317.png",
            "../../data/database/03318.png",
            "../../data/database/03421.png",
            nullptr};
    Mat image;
    vector<Point> targets;

    for (int i = 0; names[i] != nullptr; i++) {

        targets = {};

        image = imread(names[i], IMREAD_COLOR);
        cout << names[i] << endl;
        Straightener::display(image, targets);
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