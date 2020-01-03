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


        // Eroding through a 4x4 cross, which will make the center bulky and the bars super thin
        Mat cross = getStructuringElement(MORPH_CROSS,Size(4,4));
        erode(regions[i],regions[i],cross);

        // Eroding through a circle leaves only the round center
        Mat circle = getStructuringElement(MORPH_ELLIPSE,Size(3,3));
        erode(regions[i],regions[i],circle);

        //finds all the white points (aka the center of the cross) and take their average
        vector<Point> whitePoints;
        findNonZero(regions[i],whitePoints);

        Point sum = Point(0,0);
        cout << "bout to do a sum on ROI "<<i<<endl;
        for(int j=0; j < whitePoints.size(); j++){
            sum = sum + whitePoints[j];
            cout<<"I did an add"<<endl;
        }
        cout << sum << endl;
        Point average;
        if(i==0) {
            average = Point((sum.x / whitePoints.size()) + ROI_1_TL_X,
                    (sum.y / whitePoints.size()) + ROI_1_TL_Y);
        }else{
            average = Point((sum.x / whitePoints.size()) + ROI_2_TL_X,
                                  (sum.y / whitePoints.size()) + ROI_2_TL_Y);
        }

        cout << average << endl;

        targets.push_back(average);

        //display the ROI (to use only for debug)
//        namedWindow(wName, WINDOW_NORMAL);
//        resizeWindow(wName, 600,600);
//        imshow(wName, regions[i]);
//        int c = waitKey();

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
    vector<Point> targets;

    for(int i=0;names[i] != nullptr;i++) {

        targets = {};

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