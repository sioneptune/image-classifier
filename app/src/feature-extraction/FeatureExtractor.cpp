#include "feature-extraction/FeatureExtractor.h"
#include <set>

void FeatureExtractor::exportARFF(const vector<FeatureFunction> &list, const string& inputPath, const string& outputPath) {
    vector<Feature *> featureVect;
    int nbOfImages=0;
    string iname, ipath;
    string arffFile = outputPath + "extracted_images.arff";
    Mat image;


    ifstream input (inputPath + "files_output.txt");
    ofstream file (arffFile);

    if(input.is_open()) {
        if (file.is_open()) {
            cout << "Exportation to " + arffFile << endl;
            file << "@RELATION EXTRACTED_IMAGES" << endl << endl;

            // Open every image in given folder
            while(getline(input, iname)) {
                iname.replace(0,2,"");
                ipath = inputPath + iname;
                image = openImage(ipath);

                try {

                    // initialization of bounding box attributes
                    vector<Point> imgBoundingBox = boundingBox(image);
                    upLeftCorner = imgBoundingBox[0];
                    downRightCorner = imgBoundingBox[1];

                    // initialization of the extracted bounding box of the image
                    Mat bbImage = regionOfInterest(image, upLeftCorner, downRightCorner);

                    // initialization of the normalized image
                    Mat normImage = normalization(bbImage, 180);

                    // initialization of the zones
                    vector<Mat> zoneImages = zones(normImage, {2,3,2}, {2,3,2});

                    // Extraction
                    for (FeatureFunction f : list) {
                        switch (f) {
                            case BARYCENTER:
                                featureVect = barycenter(normImage);
                                results.insert(results.end(), featureVect.begin(), featureVect.end());
                                break;
                            case CONVEX_HULL_AREA:
                                results.push_back(convexHullArea(normImage, ""));
                                break;
                            case HEIGHT_WIDTH_RATIO:
                                results.push_back(heightWidthRatio());
                                break;
                            case HU_MOMENTS:
                                featureVect = HuMoments(normImage);
                                results.insert(results.end(), featureVect.begin(), featureVect.end());
                                break;
                            case LEVELS_OF_HIERARCHY:
                                results.push_back(levelsOfHierarchy(image));
                                break;
                            case NUMBER_OF_ELEMENTS:
                                results.push_back(numberOfElements(image));
                                break;
                            case LINES:
                                results.push_back(lines(normImage, MAIN_LINES_THRESHOLD));
                                break;
                            case PEAKS:
                                featureVect = peaks(normImage);
                                results.insert(results.end(), featureVect.begin(), featureVect.end());
                                break;
                            case PIXEL_RATE:
                                results.push_back(pixelRate(normImage));
                                break;

                                // Zoning
                            case ZONING_BARYCENTER:
                                featureVect = zoning_feature(zoneImages, BARYCENTER);
                                results.insert(results.end(), featureVect.begin(), featureVect.end());
                                break;
                            case ZONING_HU_MOMENTS:
                                featureVect = zoning_feature(zoneImages, HU_MOMENTS);
                                results.insert(results.end(), featureVect.begin(), featureVect.end());
                                break;
                            case ZONING_LINES:
                                featureVect = zoning_feature(zoneImages, LINES);
                                results.insert(results.end(), featureVect.begin(), featureVect.end());
                                break;
                            case ZONING_PEAKS:
                                featureVect = zoning_feature(zoneImages, PEAKS);
                                results.insert(results.end(), featureVect.begin(), featureVect.end());
                                break;
                            case ZONING_PIXEL_RATE:
                                featureVect = zoning_feature(zoneImages, PIXEL_RATE);
                                results.insert(results.end(), featureVect.begin(), featureVect.end());
                                break;
                        }
                    }

                    //Add class attribute
                    results.push_back(getClass(iname));

                    nbOfImages ++;

                } catch (Exception& e) {
                    cerr << "[FAIL] Couldn't extract features from image: " << iname << endl;
                } catch (exception& e) {
                    cerr << "C++ error on image: " << iname << ", error = " << e.what() << endl;
                }
            }

            int nbOfFeatures = results.size() / nbOfImages;

            // Export Header
            for(int i = 0; i<nbOfFeatures; i++){ file << results[i]->getDescriptor() << endl; }

            // Export Values
            file << "\n@DATA" << endl;
            for(int i = 1; i<= results.size(); i++){
                file << results[i-1]->getValue();
                if(i % nbOfFeatures == 0)    file << endl;   //End of line
                else    file << ',';                        //Separate values of the same image
            }
            file.close();
        }
        else cerr << "Unable to open output file: " << arffFile << endl;
        input.close();
    }
    else cerr << "Unable to open input file: " << iname << endl;
}


/**
=========================================================================
                          TOOLS METHODS
=========================================================================
 */

void FeatureExtractor::getContours(const Mat& image, vector<vector<Point>>& contours, vector<Vec4i>& hierarchy) const{
    Mat clean, pyr, timg, gray0, gray;
    clean = image;
    clean = removeNoise(clean);

    // down-scale and upscale the image to filter out the noise
    pyrDown(clean, pyr, Size(image.cols / 2, image.rows / 2));
    pyrUp(pyr, timg, image.size());

    threshold(timg,gray,230,255,0);

    findContours(gray, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
    //imshow("CONTOURS", gray);
    //waitKey();
}

Mat FeatureExtractor::normalization(const Mat &bbImage, const int size) const {
    Mat gray, scaled, normalized;
    Size normSize(size, size), bbSize(bbImage.cols, bbImage.rows);

    cvtColor(bbImage, gray, COLOR_BGR2GRAY);

    // Scale the image in order that the biggest side of the image matches the desired size
    int maxDim = max(bbSize.height, bbSize.width);
    double scaleFactor = (1. * size) / maxDim;
    resize(gray, scaled, Size(bbSize.width * scaleFactor, bbSize.height * scaleFactor));

    // Add borders horizontally / vertically to fill the normalized image
    int top = (normSize.height - scaled.rows) / 2;
    int down = (normSize.height - scaled.rows+1) / 2;
    int left = (normSize.width - scaled.cols) / 2;
    int right = (normSize.width - scaled.cols+1) / 2;

    copyMakeBorder(scaled, normalized, top, down, left, right, BORDER_CONSTANT, Scalar(255,255,255));

    return normalized;
}

vector<Mat> FeatureExtractor::zones(Mat &image, vector<int> decoupX, vector<int> decoupY) const{
    vector<Mat> zones(decoupX.size() * decoupY.size());

    int sizeX = image.cols;
    int sizeY = image.rows;

    vector<int> splitX, splitY;
    int sumx = 0;
    int sumy = 0;
    // Computes the sum of the elements of the split vectors, to get the proportions later
    for (int x: decoupX) sumx += x;
    for (int y : decoupY) sumy += y;

    // Computes the new sizes from the proportions
    for (int x : decoupX) splitX.push_back(x * sizeX / sumx);
    for (int y : decoupY) splitY.push_back(y * sizeY / sumy);

    vector<int> coordsX, coordsY;

    coordsX.push_back(0);
    coordsY.push_back(0);

    for (int i = 0; i < splitX.size(); i++) coordsX.push_back(coordsX[i] + splitX[i]);
    for (int i = 0; i < splitY.size(); i++) coordsY.push_back(coordsY[i] + splitY[i]);
    // We now have the coordinates to cut up the image at

//    for (int c : coordsX) cout << c << ',';
//    cout << endl;
//    for (int c : coordsY) cout << c << ',';
//    cout << endl;

    int zonesCreated = 0;
    Mat tempMat;

    for (int i = 0; i < coordsX.size() - 1; i++) {
        for (int j = 0; j < coordsY.size() - 1; j++) {
            tempMat = regionOfInterest(image, coordsX[i], coordsY[j], coordsX[i + 1], coordsY[j + 1]);
            tempMat.copyTo(zones[zonesCreated]);
            zonesCreated++;
        }
    }
    return zones;
}


/**
=========================================================================
                            FEATURES
=========================================================================
 */

vector<Feature *> FeatureExtractor::barycenter(const Mat& normImage, const string prefix) const {
    int width = normImage.cols;
    int height = normImage.rows;
    Point center = Point(width / 2, height / 2);

    vector<Point> nonzero;
    Mat binim;
    threshold(normImage, binim, 200, 255, THRESH_BINARY_INV);
    findNonZero(binim, nonzero);

    if(nonzero.size() == 0) {
        return {new FeatureDouble(prefix + "barycenter_x", 0.5), new FeatureDouble(prefix + "barycenter_y", 0.5) };
    }

    Point sum = Point(0, 0);

    for (Point p : nonzero) {
        sum += p;
    }

    Point average = Point(sum.x / nonzero.size(), sum.y / nonzero.size());

    double baryx = (double(average.x - center.x) / width) + 0.5;
    double baryy = (double(average.y - center.y) / height) + 0.5;

    if(baryx < 0) baryx = 0;
    if(baryy < 0) baryy = 0;
    if(baryx > 1) baryx = 1;
    if(baryy > 1) baryy = 1;


    FeatureDouble* baryX = new FeatureDouble(prefix + "barycenter_x", baryx);
    FeatureDouble* baryY = new FeatureDouble(prefix + "barycenter_y", baryy);
    return {baryX, baryY};
}

Feature* FeatureExtractor::convexHullArea(const Mat& normImage, const string prefix) const {
    // binarize image
    Mat binaryImage;
    // we need inverted binary threshold to no detect the frame border as a contour
    threshold(normImage, binaryImage, 230, 255, THRESH_BINARY_INV);

    // find contours
    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(binaryImage, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

    // create only one hull for all convex hull points
    vector<Point> hull;
    vector<Point> pts; // to combine all contours in one
    for (int i = 0; i < contours.size(); i++)
        for (int j = 0; j < contours[i].size(); j++)
            pts.push_back(contours[i][j]);
    convexHull(pts, hull);

    // compute the area
    double area = contourArea(hull);

//    // FOR DEBUG PURPOSE ONLY: show convex hull (blue) and contours (green)
//    Mat drawing = Mat::zeros(normImage.size(), CV_8UC3);
//    vector< vector< Point > > hullVect; // drawContours wants a vector< vector< Point > >
//    hullVect.push_back(hull);
//
//    Scalar color_contours = Scalar(0, 255, 0); // green - color for contours
//    for(int i = 0; i < contours.size(); i++) {
//        drawContours(drawing, contours, i, color_contours, 1, 8, vector<Vec4i>(), 0, Point());
//    }
//
//    drawContours(drawing, hullVect, 0, Scalar(255, 0, 0), 1, 8, vector<Vec4i>(), 0, Point());
//
//    imshow("", drawing);
//    waitKey();

    // normalization (values are between 3000 and 32000)
    double result;
    if (area > 32000) {
        result = 1.0;
    }
    else if (area < 3000) {
        result = 0.0;
    }
    else {
        result = (area - 3000) / 29000;
    }

    return new FeatureDouble(prefix + "area", result);
}

Feature* FeatureExtractor::heightWidthRatio(const string prefix) const {
    int height = downRightCorner.y - upLeftCorner. y;
    int width = downRightCorner.x - upLeftCorner.x;
    double ratio = 1.0 * height / width;

    double normalizedRatio;
    // normalization: values are between 0.2 and 4
    if(ratio > 4) {
        normalizedRatio = 1.0;
    } else if (ratio < 0.2) {
        normalizedRatio = 0.0;
    } else {
        normalizedRatio = (ratio - 0.2) / 3.8;
    }

    return new FeatureDouble(prefix + "height_width_ratio",normalizedRatio);
}

vector<Feature *> FeatureExtractor::HuMoments(const Mat& normImage, const string prefix) const {
    Mat binaryImage;
    threshold(normImage, binaryImage, 220, 255, THRESH_BINARY);

    Moments moments = cv::moments(binaryImage, false);
    double huMoments[7];
    cv::HuMoments(moments, huMoments);

    vector< vector<double> > limits = { {0.3, 0.4} ,
                                        {0.0, 0.2} ,
                                        {0.0, 0.2} ,
                                        {0.0, 0.2} ,
                                        {-0.1, 0.1} ,
                                        {-0.1, 0.1} ,
                                        {-0.1, 0.1} };
    vector<Feature*> momentFeatures;
    for (int i = 0; i<7; i++) {
        double value = 1 / ( -1 * copysign(1.0, huMoments[i]) * log10( abs(huMoments[i]) ) );

        // normalization: values are between limits[i][0] and limits[i][1]
        if (value > limits[i][1]) {
            value = 1.0;
        } else if(value < limits[i][0]) {
            value = 0.0;
        } else {
            value = (value - limits[i][0]) / (limits[i][1] - limits[i][0]);
        }

        momentFeatures.push_back(new FeatureDouble(prefix + "hu_moments_m" + to_string(i+1), value ) );
    }
    return momentFeatures;
}

Feature* FeatureExtractor::levelsOfHierarchy(const Mat& image, const string prefix) const {

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    getContours(image, contours, hierarchy);

    set<int> parents;
    // We start at 2 because contour 0 is the border of the image, and contour 1's parent is contour 0, which is useless
    for(int i = 2; i < hierarchy.size(); i++) {
        parents.insert(hierarchy[i][3]);
    }

/*    // DEBUG ONLY
    cout << parents.size() << endl;

    /// Draw contours
    Mat drawing = Mat::zeros( image.size(), CV_8UC3 );
    RNG rng(12345);
    for( int i = 0; i< contours.size(); i++ )
    {
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
    }

    imshow("draw", drawing);
    waitKey();*/

    const int levels = parents.size();
    double normalizedLevels;
    // normalization: values are between 1 and 10
    if(levels > 10) {
        normalizedLevels = 1.0;
    } else if (levels < 1) {
        normalizedLevels = 0.0;
    } else {
        normalizedLevels = (levels - 1.0) / 9.0;
    }

    return new FeatureDouble(prefix + "levels_of_hierarchy", normalizedLevels);
}

Feature* FeatureExtractor::lines(const Mat &normImage, const int threshNum, const string prefix) const {

    Mat src, dst, cdst, cdstP;
    normImage.copyTo(src);

    threshold(src,dst,230,255,THRESH_BINARY_INV);
    Mat elem = getStructuringElement(MORPH_CROSS,Size(3,3));
    dilate(dst,dst,elem);
    erode(dst,dst,elem);
    erode(dst,dst,elem);
    // Copy edges to the images that will display the results in BGR
    cvtColor(dst, cdstP, COLOR_GRAY2BGR);
    // Probabilistic Line Transform
    vector<Vec4i> linesP; // will hold the results of the detection
    HoughLinesP(dst, linesP, 1, CV_PI/180, 30, 25, 15 ); // runs the actual detection

    // Draw the lines
    //cout << "Probalistic lines:" << linesP.size() << endl;
    for( size_t i = 0; i < linesP.size(); i++ )
    {
        Vec4i l = linesP[i];
        line( cdstP, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, LINE_AA);
    }
    // Show results
//    imshow("Source", src);
//        imshow("Probabilistic Line Transform", cdstP);
//    // Wait and Exit
//    waitKey();

    double res = double(linesP.size());
    if(res > threshNum){
        res = 1.0;
    }else{
        res = res/ threshNum;
    }

    return new FeatureDouble(prefix+"lines",res);
}

Feature *FeatureExtractor::numberOfElements(const Mat &image) const {
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    getContours(image, contours, hierarchy);

    /// Draw contours
    Mat drawing = Mat::zeros( image.size(), CV_8UC3 );
    RNG rng(12345);
    for( int i = 0; i< contours.size(); i++ )
    {
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
    }

    //imshow("draw", drawing);
    //waitKey();

    return new FeatureDouble("number_of_elements", contours.size() < 10 ? float(contours.size())/10 : 1.0);
}

vector<Feature *> FeatureExtractor::peaks(const Mat &image, string prefix) const{

    // will be replaced by the actual values once we've done a few runs
    int maxpeaksX = 30;
    int maxpeaksY = 30;
    Mat binImg;
    threshold(image,binImg,230,255,THRESH_BINARY_INV);
    int sizeX = binImg.cols;
    int sizeY = binImg.rows;
    vector<int> h_histo(sizeX,0);
    vector<int> v_histo(sizeY,0);

    for(int i=0;i<sizeX;i++){
        for(int j=0;j<sizeY;j++){
            if(binImg.at<int>(i,j) > 0) {
                h_histo[i] +=1;
                v_histo[j] += 1;
            }
        }
    }

    vector<int> peaksX = histopeaks(h_histo);
    vector<int> peaksY = histopeaks(v_histo);




    FeatureDouble* peaknumX = new FeatureDouble(prefix + "peaks_x", peaksX.size() > maxpeaksX ? 1.0 : (double)peaksX.size()/(double)maxpeaksX);
    FeatureDouble* peaknumY = new FeatureDouble(prefix + "peaks_y", peaksY.size() > maxpeaksY ? 1.0 : (double)peaksY.size()/(double)maxpeaksY);

    return {peaknumX, peaknumY};
}

Feature* FeatureExtractor::pixelRate(const Mat& normImage, const string prefix) const {
    Mat binaryImage;
    threshold(normImage, binaryImage, 220, 255, THRESH_BINARY);

    vector<Point> whitePoints;
    findNonZero(binaryImage, whitePoints);

    int nbOfPixels = binaryImage.cols * binaryImage.rows;
    int nbOfBlackPixels = nbOfPixels - whitePoints.size();

    return new FeatureDouble(prefix + "drawing_pixel_rate_on_image", (1.0 * nbOfBlackPixels / nbOfPixels));
}

vector<Feature *> FeatureExtractor::zoning_feature(const vector<Mat> zoneImages, const FeatureFunction f) const {
    vector<Feature *> results;
    vector<Feature *> tmp;
    for(int i=0; i<zoneImages.size(); i++) {
        switch(f) {
            case BARYCENTER:
                tmp = barycenter(zoneImages[i], "zone_" + to_string(i) + "_");
                results.insert(results.end(), tmp.begin(), tmp.end());
                break;
            case PIXEL_RATE:
                results.push_back(pixelRate(zoneImages[i], "zone_" + to_string(i) + "_"));
                break;
            case HU_MOMENTS:
                tmp = HuMoments(zoneImages[i], "zone_" + to_string(i) + "_");
                results.insert(results.end(), tmp.begin(), tmp.end());
                break;
            case LINES:
                results.push_back(lines(zoneImages[i], ZONE_LINES_THRESHOLD, "zone_" + to_string(i) + "_"));
                break;
            case PEAKS:
                tmp = peaks(zoneImages[i], "zone_" + to_string(i) + "_");
                results.insert(results.end(), tmp.begin(), tmp.end());
                break;
        }
    }
    return results;
}

Feature *FeatureExtractor::getClass(const string name) const {
    unsigned stop = name.find('/');
    return new FeatureString("class","{accident, bomb, car, casualty, electricity, fire, fire_brigade, flood, gas, injury, paramedics, person, police, road_block}", name.substr(0, stop));
}


/**
=========================================================================
                          MAIN
=========================================================================
 */

int main() {
    FeatureExtractor feat;
    feat.exportARFF({ BARYCENTER,
                      CONVEX_HULL_AREA,
                      HEIGHT_WIDTH_RATIO,
                      PIXEL_RATE,
                      LEVELS_OF_HIERARCHY,
                      HU_MOMENTS,
                      NUMBER_OF_ELEMENTS,
                      LINES,
                      PEAKS,
                      // ZONES
                      ZONING_BARYCENTER,
                      ZONING_PIXEL_RATE,
                      ZONING_HU_MOMENTS,
                      ZONING_LINES,
                      ZONING_PEAKS }, "../../data/output/", "../../data/");
}
