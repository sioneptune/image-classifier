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
    vector<string> names = {"../../data/output/accident/accident_000_00_1_4.png",
                            "../../data/output/bomb/bomb_000_04_5_4.png",
                            "../../data/output/car/car_000_02_6_2.png",
                            "../../data/output/casualty/casualty_000_03_2_4.png",
                            "../../data/output/electricity/electricity_000_00_5_2.png",
                            "../../data/output/fire/fire_000_00_6_2.png",
                            "../../data/output/fire_brigade/fire brigade_000_03_4_2.png",
                            "../../data/output/flood/flood_000_04_1_3.png",
                            "../../data/output/gas/gas_000_03_7_1.png",
                            "../../data/output/injury/injury_000_03_5_3.png",
                            "../../data/output/paramedics/paramedics_000_04_4_3.png",
                            "../../data/output/person/person_000_02_3_5.png",
                            "../../data/output/police/police_000_02_4_4.png",
                            "../../data/output/road_block/road block_000_02_5_1.png"};
    FeatureExtractor f = FeatureExtractor();
    Mat image;
    for(string name : names){
        image = imread(name);
        f.setImage(image);
        f.lines(image);
    }
}

int __main() {
    FeatureExtractor feat;
    feat.exportARFF({FUNCTION_BOOL, FUNCTION_INT, FUNCTION_STRING, FUNCTION_DOUBLE, BARYCENTER_X, BARYCENTER_Y}, "../../data/output/",
                    "../../data/");
}
