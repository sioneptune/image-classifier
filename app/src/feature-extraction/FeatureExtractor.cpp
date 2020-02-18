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
                    vector<Mat> zoneImages = zones(normImage, {1,1,1}, {1,1,1});

                    // Extraction
                    for (FeatureFunction f : list) {
                        switch (f) {
                            case BARYCENTER:
                                featureVect = barycenter(normImage);
                                results.insert(results.end(), featureVect.begin(), featureVect.end());
                                break;
                            case HEIGHT_WIDTH_RATIO:
                                results.push_back(heightWidthRatio());
                                break;
                            case LEVELS_OF_HIERARCHY:
                                results.push_back(levelsOfHierarchy(image));
                                break;
                            case PIXEL_RATE:
                                results.push_back(pixelRate(normImage));
                                break;
                            case HU_MOMENTS:
                                featureVect = HuMoments(normImage);
                                results.insert(results.end(), featureVect.begin(), featureVect.end());
                                break;
                            case LINES:
                                results.push_back(lines(normImage));
                                break;
                            case NUMBER_OF_ELEMENTS:
                                results.push_back(numberOfElements(image));
                                break;

                                // Zoning
                            case ZONING_BARYCENTER:
                                featureVect = zoning_feature(zoneImages, BARYCENTER);
                                results.insert(results.end(), featureVect.begin(), featureVect.end());
                                break;
                            case ZONING_PIXEL_RATE:
                                featureVect = zoning_feature(zoneImages, PIXEL_RATE);
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

vector<Feature *> FeatureExtractor::barycenter(const Mat& normImage, const string prefix) const {
    Point center = Point((downRightCorner.x + upLeftCorner.x) / 2, (upLeftCorner.y + downRightCorner.y) / 2);

    vector<Point> nonzero;
    Mat binim;
    threshold(normImage, binim, 200, 255, THRESH_BINARY_INV);
    findNonZero(binim, nonzero);

    if(nonzero.size() == 0) {
        return {new FeatureDouble(prefix + "barycenter_x", center.x), new FeatureDouble(prefix + "barycenter_y", center.y) };
    }

    Point sum = Point(0, 0);

    for (Point p : nonzero) {
        sum += p;
    }

    Point average = Point(sum.x / nonzero.size(), sum.y / nonzero.size());

    double baryx = (double) (average.x - center.x) / ((double) (upLeftCorner.x - downRightCorner.x));
    double baryy = (double) (average.y - center.y) / ((double) (upLeftCorner.y - downRightCorner.y));


    FeatureDouble* baryX = new FeatureDouble(prefix + "barycenter_x", baryx);
    FeatureDouble* baryY = new FeatureDouble(prefix + "barycenter_y", baryy);

    vector<Feature*> res = {baryX, baryY};
    return res;
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

Feature* FeatureExtractor::pixelRate(const Mat& normImage, const string prefix) const {
    Mat binaryImage;
    threshold(normImage, binaryImage, 220, 255, THRESH_BINARY);

    vector<Point> whitePoints;
    findNonZero(binaryImage, whitePoints);

    int nbOfPixels = binaryImage.cols * binaryImage.rows;
    int nbOfBlackPixels = nbOfPixels - whitePoints.size();

    return new FeatureDouble(prefix + "drawing_pixel_rate_on_image", (1.0 * nbOfBlackPixels / nbOfPixels));
}

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

vector<Feature *> FeatureExtractor::HuMoments(const Mat& normImage, const string prefix) const {
    Mat binaryImage;
    threshold(normImage, binaryImage, 220, 255, THRESH_BINARY);

    Moments moments = cv::moments(binaryImage, false);
    double huMoments[7];
    cv::HuMoments(moments, huMoments);

    vector<Feature*> momentFeatures;
    for(int i = 0; i<7; i++) {
        momentFeatures.push_back(new FeatureDouble(prefix + "hu_moments_m" + to_string(i+1), 1 / ( -1 * copysign(1.0, huMoments[i]) * log10( abs(huMoments[i]) ) ) ) );
    }
    return momentFeatures;
}

Feature* FeatureExtractor::lines(const Mat &normImage, const string prefix) const {

    Mat src, dst, cdst, cdstP;
    normImage.copyTo(src);

/*    // Edge detection
    Canny(src, dst, 50, 200, 3);*/

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

    Feature * f = new FeatureInt(prefix+"lines",linesP.size());

    return f;
}

vector<Mat> FeatureExtractor::zones(Mat &image, vector<int> decoupX, vector<int> decoupY) {
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
                results.push_back(lines(zoneImages[i], "zone_" + to_string(i) + "_"));
                break;
        }
    }
    return results;
}

Feature *FeatureExtractor::numberOfElements(const Mat &normImage) const {
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    getContours(normImage, contours, hierarchy);

    /// Draw contours
    Mat drawing = Mat::zeros( normImage.size(), CV_8UC3 );
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

Feature *FeatureExtractor::getClass(const string name) const {
    unsigned stop = name.find('/');
    return new FeatureString("class","{accident, bomb, car, casualty, electricity, fire, fire_brigade, flood, gas, injury, paramedics, person, police, road_block}", name.substr(0, stop));
}


int main() {
    FeatureExtractor feat;
    feat.exportARFF({ZONING_BARYCENTER, ZONING_PIXEL_RATE, ZONING_HU_MOMENTS, NUMBER_OF_ELEMENTS}, "../../data/output/", "../../data/");
}
