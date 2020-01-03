//
// Created by theotime on 03/01/2020.
//

#include "Straigthener.h"
#include "tools.h"

void Straigthener::processImage(Mat &image) {
    Mat tiny, clean;

    pyrDown(image, tiny, Size(image.cols/2, image.rows/2));
    pyrUp(tiny,clean, image.size());
    cvtColor(clean, image, COLOR_BGR2GRAY);
}

void Straigthener::findTargets(Mat &image, vector<Point> &targets) {
    Mat analyze;
    image.copyTo(analyze);
    Mat ro1 = regionOfInterest(analyze, Point(ROI_1_TL_X,ROI_1_TL_Y), Point(ROI_1_BR_X,ROI_1_BR_Y));
    Mat ro2 = regionOfInterest(analyze, Point(ROI_2_TL_X,ROI_2_TL_Y), Point(ROI_2_BR_X,ROI_2_BR_Y));
    vector<Mat> regions = {ro1,ro2};
    for(int i = 0; i < regions.size();i++){
        threshold(regions[i], regions[i],200,255,THRESH_BINARY_INV);

        string wName = to_string(i);
        namedWindow(wName, WINDOW_NORMAL);
        resizeWindow(wName, 600,600);
        imshow(wName, regions[i]);

        int c = waitKey();

    }
}

void Straigthener::display(Mat &image, vector<Point> &targets) {
    Mat disp;
    image.copyTo(disp);
    for(int i=0;i < targets.size();i++){
        circle(disp,targets[i],100,Scalar(0,0,255),2);
    }
    const string wName = "Results";

    namedWindow(wName, WINDOW_NORMAL);
    resizeWindow(wName, 600,600);
    imshow(wName, disp);
}

int main(){
    static const char *names[] = {"../../../data/00000.png", "../../../data/00000_rotate.png", nullptr};

    Mat image;
    vector<Point> targets = {Point(100, 100), Point(500, 500)};

    for(int i=0;names[i] != nullptr;i++) {

        image = imread(names[i], IMREAD_COLOR);
        cout << "image size " << image.size << endl;
        if (image.empty()) {
            cout << "Couldn't load " << names[i] << endl;
            exit(0);
        }

        Straigthener::processImage(image);
        cout << "image size " << image.size << endl;

        Straigthener::findTargets(image,targets);

        Straigthener::display(image, targets);

        int c = waitKey();
    }
}