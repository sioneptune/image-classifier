//
// Created by theotime on 03/01/2020.
//

#include "Straigthener.h"
#include "tools.h"

void Straigthener::processImage(Mat &image) {
    Mat tiny, clean;

    pyrDown(image, tiny, Size(image.cols/2, image.rows/2));
    pyrDown(tiny,clean, image.size());
    cvtColor(clean, image, COLOR_BGR2GRAY);
}

void Straigthener::findTargets(Mat &image, vector<Point> &targets) {

}

void Straigthener::display(Mat &image, vector<Point> &targets) {
    for(int i=0;i < targets.size();i++){
        circle(image,targets[i],10,Scalar(0,0,255));
    }
    const string wName = "Results";

    namedWindow(wName, WINDOW_NORMAL);
    resizeWindow(wName, 600,600);
    imshow(wName, image);
}

int main(){
    static const string name = "../../../data/00000.png";
    Mat image = imread(name, IMREAD_COLOR);

    if (image.empty()) {
        cout << "Couldn't load " << name << endl;
        exit(0);
    }

    Straigthener::processImage(image);

    vector<Point> targets = {};


}