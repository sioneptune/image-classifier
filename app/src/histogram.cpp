#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include "histogram.h"


void computeHistogram(const string &histTitle, const Mat &img) {

    // INIT
    int histSize = 255; // Number of bins
    float range[] = {0, 255}; // Range of pixel values
    const float *histRange = {range};
    bool uniform = true;
    bool accumulate = false;

    // Number of histograms to compute (1: grayLevel or 3: BGR)
    int numChannels = img.channels();
    MatND *hist = new MatND[numChannels];

    // Split BRG if needed
    vector<Mat> channels;
    if (numChannels == 3) split(img, channels);
    else channels.push_back(img);

    // COMPUTE
    int zero = 0;
    for (int idxChannel = 0; idxChannel < numChannels; idxChannel++) {
        calcHist(&channels[idxChannel], 1, &zero, Mat(), hist[idxChannel], 1, &histSize, &histRange, uniform,
                 accumulate);
    }

    // DISPLAY
    int hist_w = 1000;
    int hist_h = 600; // Size of the histogram's image
    int bin_w = cvRound((double) hist_w / (double) histSize); // Width of a bin
    Mat histImage(hist_h, hist_w, CV_8UC3, CV_RGB(0, 0, 0));

    Point pt1, pt2;
    Scalar color;
    double scale, max;

    // For each channel
    for (int idxChannel = 0; idxChannel < numChannels; idxChannel++) {
        // Scale value
        minMaxLoc(hist[idxChannel], 0, &max, 0, 0);
        scale = (double) hist_h / (max + 500);

        // Color of the line (B G R or white)
        if (numChannels == 1)
            color = CV_RGB(255, 255, 255); // White
        else {
            if (idxChannel == 0) color = CV_RGB(0, 0, 255); // Blue
            else if (idxChannel == 1) color = CV_RGB(0, 255, 0); // Green
            else color = CV_RGB(255, 0, 0); // Red
        }

        // For each bin
        for (int idxBin = 1; idxBin < histSize; idxBin++) {

            // Draw a line
            pt1 = Point(bin_w * (idxBin - 1),
                        cvRound(float(hist_h) - (hist[idxChannel].at<float>(idxBin - 1)) * scale));
            pt2 = Point(bin_w * (idxBin), cvRound(float(hist_h) - (hist[idxChannel].at<float>(idxBin)) * scale));
            line(histImage, pt1, pt2, color, 2, 8, 0);

            /*
            line( histImage,
                        Point( bin_w*idxBin, hist_h ) ,
                        Point( bin_w*idxBin, hist_h - cvRound(hist[idxChannel].at<float>(idxBin)) ),
                        CV_RGB(255,255,255));
            */
        }
    }
    imshow(histTitle, histImage);

    //waitKey(0);

    delete[]hist;
}