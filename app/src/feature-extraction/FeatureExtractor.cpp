#include "feature-extraction/FeatureExtractor.h"

FeatureExtractor::~FeatureExtractor() {
    for (auto f: results) {
        delete f;
    }
}

void FeatureExtractor::setImage(const Mat &img) {
    image = img;
}

Feature *FeatureExtractor::functionBool() const {
    return new FeatureBool(FUNCTION_BOOL, false);
}

Feature *FeatureExtractor::functionString() const {
    return new FeatureString(FUNCTION_STRING, "{pelican, pingouin, cockatiel}", "cockatiel");
}

Feature *FeatureExtractor::functionInt() const {
    return new FeatureInt(FUNCTION_INT, 124);
}

Feature *FeatureExtractor::functionDouble() const {
    return new FeatureDouble(FUNCTION_DOUBLE, -34.56);
}

void
FeatureExtractor::exportARFF(const vector<FeatureFunction> &list, const string inputPath, const string outputPath) {
    Feature *feat = nullptr;
    string iname;
    string name = outputPath + "extracted_images.arff";
    Mat img;


    ifstream input(inputPath + "files_output.txt");
    ofstream file(name);

    if (input.is_open()) {
        if (file.is_open()) {
            cout << "Exportation to " + name << endl;
            file << "@RELATION EXTRACTED_IMAGES" << endl << endl;

            // Open every image in given folder
            while (getline(input, iname)) {
                iname = inputPath + iname;
                cout << "Iname : " + iname << endl;
                img = openImage(iname);

                // initialization of image attribute
                setImage(img);

                // initialization of bouding box attributes
                vector<Point> imgBoundingBox = boundingBox(image);
                upLeftCorner = imgBoundingBox[0];
                downRightCorner = imgBoundingBox[1];

                // Extraction
                for (FeatureFunction f : list) {
                    switch (f) {
                        case FUNCTION_BOOL:
                            feat = functionBool();
                            break;
                        case FUNCTION_STRING:
                            feat = functionString();
                            break;
                        case FUNCTION_INT:
                            feat = functionInt();
                            break;
                        case FUNCTION_DOUBLE:
                            feat = functionDouble();
                            break;
                    }
                    results.push_back(feat);
                }
            }
            // Export Header
            for (int i = 0; i < list.size(); i++) { file << results[i]->getDescriptor() << endl; }

            // Export Values
            file << "\n@DATA" << endl;
            for (int i = 1; i <= results.size(); i++) {
                file << results[i - 1]->getValue();
                if (i % list.size() == 0) file << endl;   //End of line
                else file << ',';                        //Separate values of the same image
            }
            file.close();
        } else cerr << "Unable to open file: " << name << endl;
        input.close();
    } else cerr << "Unable to open file: " << name << endl;
}

vector<Feature *> FeatureExtractor::barycentre(Mat &image) {
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

    FeatureDouble baryX = FeatureDouble(BARYCENTER_X, baryx);
    FeatureDouble baryY = FeatureDouble(BARYCENTER_Y, baryy);

    vector<Feature *> res = {&baryX, &baryY};
    return res;
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
    cvtColor(dst, cdst, COLOR_GRAY2BGR);
    cdstP = cdst.clone();
    // Standard Hough Line Transform
    vector<Vec2f> lines; // will hold the results of the detection
    HoughLines(dst, lines, 1, CV_PI/180, 55, 0, 0 ); // runs the actual detection
    // Draw the lines
    for( size_t i = 0; i < lines.size(); i++ )
    {
        float rho = lines[i][0], theta = lines[i][1];
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = cvRound(x0 + 1000*(-b));
        pt1.y = cvRound(y0 + 1000*(a));
        pt2.x = cvRound(x0 - 1000*(-b));
        pt2.y = cvRound(y0 - 1000*(a));
        line( cdst, pt1, pt2, Scalar(0,0,255), 3, LINE_AA);
    }
    // Probabilistic Line Transform
    vector<Vec4i> linesP; // will hold the results of the detection
    HoughLinesP(dst, linesP, 1, CV_PI/180, 30, 25, 10 ); // runs the actual detection

        // Draw the lines
        cout << linesP.size() << endl;
    for( size_t i = 0; i < linesP.size(); i++ )
    {
        Vec4i l = linesP[i];
        line( cdstP, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, LINE_AA);
    }
    // Show results
    imshow("Source", src);
    imshow("Standard Hough Line Transform", cdst);
    imshow("Probabilistic Line Transform", cdstP);
    // Wait and Exit
    waitKey();

    return nullptr;
}

int main() {
    FeatureExtractor feat;
    feat.exportARFF({FUNCTION_BOOL, FUNCTION_INT, FUNCTION_STRING, FUNCTION_DOUBLE}, "../../data/output/",
                    "../../data/");
}
