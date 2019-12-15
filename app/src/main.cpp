#include <iostream>

using namespace std;

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

using namespace cv;

#include "histogram.h"
#include "tools.h"

int main(void) {

    //charge et affiche l'image (� MODIFIER) :
    string imName = "../../../data/Lenna.png";
    Mat im = imread(imName); // FIXME imread opens in BGR !
    if (im.data == nullptr) {
        cerr << "Image not found: " << imName << endl;
        waitKey(0);
        //system("pause");
        exit(EXIT_FAILURE);
    }
    imshow("exemple1", im);

    //applique une reduction de taille d'un facteur 5
    //ici modifier pour ne reduire qu'a l'affichage
    //comme demande dans l'enonce
    int reduction = 5;
    Size tailleReduite(im.cols / reduction, im.rows / reduction);
    Mat imreduite = Mat(tailleReduite, CV_8UC3); //cree une image � 3 canaux de profondeur 8 bits chacuns
    resize(im, imreduite, tailleReduite);
    imshow("image reduite", imreduite);

    computeHistogram("histogramme", im);

    //Convert to HSV
    Mat imHSV = Mat(Size(im.cols, im.rows), CV_8UC3);
    cvtColor(im, imHSV, COLOR_BGR2HSV, 0);

    imshow("HSV", imHSV);

    //Split HSV channels and display them
    int numChannels = imHSV.channels();
    vector<Mat> channels;
    if (numChannels == 3) split(imHSV, channels);
    else channels.push_back(imHSV);

    int i = 0;
    Mat channelThreshed;
    for (Mat channelHSV : channels) {
        imshow("HSV" + std::to_string(i), channelHSV);
        i++;
        //Thresholding
        if (i == 2) {
            channelThreshed = Mat(Size(channelHSV.cols, channelHSV.rows), CV_8UC3);
            threshold(channelHSV, channelThreshed, 180.f, 100.f, THRESH_TOZERO);
            imshow("thresh", channelThreshed);
        }
    }

    //ROI
    //Rect roi = Rect(240, 245, 130, 40);
    Mat roiImg = regionOfInterest(channelThreshed, Point(240,245), Point(240+130, 245+40));
    imshow("roi", roiImg);

    //Save img
    vector<int> compression_params;
    compression_params.push_back(IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);

    imwrite("../../../data/Lenneys.png", roiImg, compression_params);

    //termine le programme lorsqu'une touche est frappee
    waitKey(0);
    //system("pause");
    return EXIT_SUCCESS;
}
