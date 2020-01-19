#include "feature-extraction/FeatureExtractor.h"

FeatureExtractor::FeatureExtractor() {
    results = vector<Feature*>();
}

Feature* FeatureExtractor::functionBool(Mat& img) {
    return new FeatureBool(FUNCTION_BOOL, false);
}

Feature* FeatureExtractor::functionString(Mat& img) {
    return new FeatureString(FUNCTION_STRING, "{pelican, pingouin, cockatiel}", "cockatiel");
}

Feature* FeatureExtractor::functionInt(Mat& img) {
    return new FeatureInt(FUNCTION_INT, 124);
}

Feature* FeatureExtractor::functionDouble(Mat& img) {
    return new FeatureDouble(FUNCTION_DOUBLE, -34.56);
}

Feature* FeatureExtractor::nbCircles(const Mat& img) {
    // Image pre-processing
    Mat modif;
    cvtColor(img, modif, COLOR_BGR2GRAY);
    morphologyEx(modif, modif, MORPH_ERODE, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));
//    GaussianBlur(modif, modif, Size(9, 9), 2, 2);

    vector<Point> bb = boundingBox(img);
    const int width = abs(bb[0].x - bb[1].x);
    const int ARBITRARY_THRESH_HOUGH = 10;
    const int nbPointsNeeded = width + ARBITRARY_THRESH_HOUGH;

    vector<Vec3f> circles;
    HoughCircles(modif, circles,HOUGH_GRADIENT,2, modif.rows/4,100, nbPointsNeeded);

    /* DEBUG */
    /// Draw the circles detected
    for( size_t i = 0; i < circles.size(); i++ )
    {
        Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
        // circle center
        circle( img, center, 3, Scalar(0,255,0), -1, 8, 0 );
        // circle outline
        circle( img, center, radius, Scalar(0,0,255), 3, 8, 0 );
    }

    /// Show your results
    namedWindow( "Hough Circle Transform Demo", CV_WINDOW_AUTOSIZE );
    imshow( "Hough Circle Transform Demo", img );
    waitKey();

    return new FeatureInt(NB_CIRCLES, circles.size());
}

void FeatureExtractor::exportARFF(const vector<FeatureFunction> &list, string inputPath, string outputPath) {
    Feature *feat = nullptr;
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
                iname = inputPath + iname;
                cout << "Iname : " + iname << endl;
                img = openImage(iname);

                // Extraction
                for (FeatureFunction f : list) {
                    switch (f) {
                        case FUNCTION_BOOL:     feat = functionBool(img);
                            break;
                        case FUNCTION_STRING:   feat = functionString(img);
                            break;
                        case FUNCTION_INT:      feat = functionInt(img);
                            break;
                        case FUNCTION_DOUBLE:   feat = functionDouble(img);
                            break;
                    }
                    results.push_back(feat);
                }
            }
            // Export Header
            for(int i = 0; i<list.size(); i++){ file << results[i]->getDescriptor() << endl; }

            // Export Values
            file << "\n@DATA" << endl;
            for(int i = 1; i<= results.size(); i++){
                file << results[i-1]->getValue();
                if(i % list.size() == 0)    file << endl;   //End of line
                else    file << ',';                        //Separate values of the same image
            }
            file.close();
        }
        else cerr << "Unable to open file: " << name << endl;
        input.close();
    }
    else cerr << "Unable to open file: " << name << endl;
}

int main(){
    FeatureExtractor feat = FeatureExtractor();
//    feat.exportARFF({ FUNCTION_BOOL, FUNCTION_INT, FUNCTION_STRING, FUNCTION_DOUBLE }, "../../data/output/", "../../data/");

    /* TESTS FOR NBCIRCLES */
//    // Bomb
//    Mat i = openImage("../../data/output/bomb/bomb_000_18_2_4.png");
//    cout << feat.nbCircles(i)->getValue() << endl;
//    i = openImage("../../data/output/bomb/bomb_002_12_1_3.png");
//    cout << feat.nbCircles(i)->getValue() << endl;
//    i = openImage("../../data/output/bomb/bomb_034_09_5_3.png");
//    cout << feat.nbCircles(i)->getValue() << endl;
//
//    // Casualty
//    Mat i = openImage("../../data/output/casualty/casualty_003_02_6_3.png");
//    cout << feat.nbCircles(i)->getValue() << endl;
//    i = openImage("../../data/output/casualty/casualty_002_02_7_2.png");
//    cout << feat.nbCircles(i)->getValue() << endl;
//    i = openImage("../../data/output/casualty/casualty_001_03_5_1.png");
//    cout << feat.nbCircles(i)->getValue() << endl;
//    i = openImage("../../data/output/casualty/casualty_003_18_5_4.png");
//    cout << feat.nbCircles(i)->getValue() << endl;
//
//    // Injury
//    Mat i = openImage("../../data/output/injury/injury_000_01_2_1.png");
//    cout << feat.nbCircles(i)->getValue() << endl;
//    i = openImage("../../data/output/injury/injury_000_04_3_2.png");
//    cout << feat.nbCircles(i)->getValue() << endl;
//    i = openImage("../../data/output/injury/injury_000_15_2_3.png");
//    cout << feat.nbCircles(i)->getValue() << endl;
//    i = openImage("../../data/output/injury/injury_001_09_2_5.png");
//    cout << feat.nbCircles(i)->getValue() << endl;
//
//    // Road_block
//    Mat i = openImage("../../data/output/road_block/road block_000_01_6_2.png");
//    cout << feat.nbCircles(i)->getValue() << endl;
//    i = openImage("../../data/output/road_block/road block_000_14_4_1.png");
//    cout << feat.nbCircles(i)->getValue() << endl;
//    i = openImage("../../data/output/road_block/road block_001_05_4_2.png");
//    cout << feat.nbCircles(i)->getValue() << endl;
//    i = openImage("../../data/output/road_block/road block_004_03_5_4.png");
//    cout << feat.nbCircles(i)->getValue() << endl;
//    i = openImage("../../data/output/road_block/road block_005_06_3_1.png");
//    cout << feat.nbCircles(i)->getValue() << endl;

}
