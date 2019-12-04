#ifndef HISTOGRAM_H_
#define HISTOGRAM_H_

#include <iostream>
using namespace std;

#include "opencv2/imgproc.hpp"

using namespace cv;

/*
* Compute and show histogram of a gray OR color BGR image
*/
void computeHistogram(const string& histTitle,const Mat& img);


#endif /* HISTOGRAM_H_ */