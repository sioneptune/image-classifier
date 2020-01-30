#include <set>
#include "feature-extraction/FeatureExtractor.h"
void FeatureExtractor::exportARFF(const vector<FeatureFunction> &list, const string& inputPath, const string& outputPath) {

    vector<Feature *> featureVect;
    int nbOfImages=0;
    string iname;
    string name = outputPath + "extracted_images.arff";
    Mat img;


    ifstream input (inputPath + "files_output.txt");
    ofstream file (name);

    if(input.is_open()) {
        if (file.is_open()) {
            cout << "Exportation to " + name << endl;
            file << "@RELATION EXTRACTED_IMAGES" << endl << endl;

            // Open every image in given folder
            while(getline(input, iname)) {
                iname.replace(0,2,"");
                iname = inputPath + iname;
                img = openImage(iname);

                // initialization of image attribute
                setImage(img);

                try {

                    // initialization of bounding box attributes
                    vector<Point> imgBoundingBox = boundingBox(image);
                    upLeftCorner = imgBoundingBox[0];
                    downRightCorner = imgBoundingBox[1];

                    // initialization of the extracted bounding box of the image
                    setBBImage(regionOfInterest(img, upLeftCorner, downRightCorner));

                    // Extraction
                    for (FeatureFunction f : list) {
                        switch (f) {
                            case BARYCENTER:
                                featureVect = barycenter();
                                results.insert(results.end(), featureVect.begin(), featureVect.end());
                                break;
                            case HEIGHT_WIDTH_RATIO:
                                results.push_back(heightWidthRatio());
                                break;
                            case LEVELS_OF_HIERARCHY:
                                results.push_back(levelsOfHierarchy());
                                break;
                            case PIXEL_RATE:
                                results.push_back(pixelRate());
                                break;
                            case HU_MOMENTS:
                                featureVect = HuMoments();
                                results.insert(results.end(), featureVect.begin(), featureVect.end());
                                break;
                        }
                    }
                } catch (Exception& e) {
                    cerr << "[FAIL] Couldn't extract features from image: " << iname << endl;
                } catch (exception& e) {
                    cerr << "C++ error on image: " << iname << ", error = " << e.what() << endl;
                }

                nbOfImages ++;
            }
            // Export Header
            for(int i = 0; i<(results.size() / nbOfImages); i++){ file << results[i]->getDescriptor() << endl; }

            // Export Values
            file << "\n@DATA" << endl;
            for(int i = 1; i<= results.size(); i++){
                file << results[i-1]->getValue();
                if(i % (results.size() / nbOfImages) == 0)    file << endl;   //End of line
                else    file << ',';                        //Separate values of the same image
            }
            file.close();
        }
        else cerr << "Unable to open file: " << name << endl;
        input.close();
    }
    else cerr << "Unable to open file: " << iname << endl;
}

vector<Feature *> FeatureExtractor::barycenter() const {
    vector<Point> nonzero;
    Mat binim;
    cvtColor(image, binim, COLOR_BGR2GRAY);
    threshold(binim, binim, 200, 255, THRESH_BINARY_INV);
    findNonZero(binim, nonzero);
    Point sum = Point(0, 0);

    for (Point p : nonzero) {
        sum += p;
    }

    Point average = Point(sum.x / nonzero.size(), sum.y / nonzero.size());
    Point center = Point((downRightCorner.x + upLeftCorner.x) / 2, (upLeftCorner.y + downRightCorner.y) / 2);
    double baryx = (double) (average.x - center.x) / ((double) (upLeftCorner.x - downRightCorner.x));
    double baryy = (double) (average.y - center.y) / ((double) (upLeftCorner.y - downRightCorner.y));

    FeatureDouble* baryX = new FeatureDouble("barycenter_x", baryx);
    FeatureDouble* baryY = new FeatureDouble("barycenter_y", baryy);

    vector<Feature*> res = {baryX, baryY};
    return res;
}

Feature* FeatureExtractor::heightWidthRatio() const {
    int height = downRightCorner.y - upLeftCorner. y;
    int width = downRightCorner.x - upLeftCorner.x;
    return new FeatureDouble("height_width_ratio",1.0 * height / width );
}

Feature* FeatureExtractor::pixelRate() const {
    Mat binaryImage;
    cvtColor(bbImage, binaryImage, COLOR_BGR2GRAY);
    threshold(bbImage, binaryImage, 220, 255, THRESH_BINARY);
    cvtColor(binaryImage, binaryImage, COLOR_BGR2GRAY);

    vector<Point> whitePoints;
    findNonZero(binaryImage, whitePoints);

    int nbOfPixels = binaryImage.cols * binaryImage.rows;
    int nbOfBlackPixels = nbOfPixels - whitePoints.size();

    return new FeatureDouble("drawing_pixel_rate_on_image", (1.0 * nbOfBlackPixels / nbOfPixels));
}

Feature* FeatureExtractor::levelsOfHierarchy() const {

    Mat clean, pyr, timg, gray0, gray;

    clean = removeNoise(image);

    // down-scale and upscale the image to filter out the noise
    pyrDown(clean, pyr, Size(image.cols / 2, image.rows / 2));
    pyrUp(pyr, timg, image.size());

    threshold(timg,gray,230,255,0);

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    // find contours and store them all as a list
    findContours(gray, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    set<int> parents;
    // We start at 2 because contour 0 is the border of the image, and contour 1's parent is contour 0, which is useless
    for(int i = 2; i < hierarchy.size(); i++) {
        parents.insert(hierarchy[i][3]);
    }

/*
    // DEBUG ONLY
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
    waitKey();
*/

    return new FeatureInt("levels_of_hierarchy", parents.size());
}

vector<Feature *> FeatureExtractor::HuMoments() const {
    Mat binaryImage;
    cvtColor(bbImage, binaryImage, COLOR_BGR2GRAY);
    threshold(binaryImage, binaryImage, 220, 255, CV_THRESH_BINARY);

    Moments moments = cv::moments(binaryImage, false);
    double huMoments[7];
    cv::HuMoments(moments, huMoments);

    vector<Feature*> momentFeatures;
    for(int i = 0; i<7; i++) {
        momentFeatures.push_back(new FeatureDouble("hu_moments_m" + to_string(i+1), -1 * copysign(1.0, huMoments[i]) * log10(abs(huMoments[i]))));
    }

    return momentFeatures;
}

Feature *FeatureExtractor::lines(Mat &image) {

    Mat src, dst, cdst, cdstP;
    image.copyTo(src);

/*    // Edge detection
    Canny(src, dst, 50, 200, 3);*/

    cvtColor(src,dst,COLOR_BGR2GRAY);
    threshold(dst,dst,230,255,THRESH_BINARY_INV);
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
        cout << "Probalistic lines:" << linesP.size() << endl;
    for( size_t i = 0; i < linesP.size(); i++ )
    {
        Vec4i l = linesP[i];
        line( cdstP, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, LINE_AA);
    }
    // Show results
    imshow("Source", src);
        imshow("Probabilistic Line Transform", cdstP);
    // Wait and Exit
    waitKey();

    return nullptr;
}

int main(){
    FeatureExtractor feat;
    feat.exportARFF({BARYCENTER, HEIGHT_WIDTH_RATIO, PIXEL_RATE, LEVELS_OF_HIERARCHY, HU_MOMENTS}, "../../data/output/", "../../data/");
}
