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
    Mat modif;
    cvtColor(img, modif, COLOR_BGR2GRAY);
    morphologyEx(modif, modif, MORPH_ERODE, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));
//    GaussianBlur(modif, modif, Size(9, 9), 2, 2);

    imshow("modif", modif);

    vector<Vec3f> circles;
    HoughCircles(modif, circles,HOUGH_GRADIENT,2, modif.rows/4,100,100);

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
    feat.exportARFF({ FUNCTION_BOOL, FUNCTION_INT, FUNCTION_STRING, FUNCTION_DOUBLE }, "../../data/output/", "../../data/");
    Mat i = openImage("../../data/output/bomb/bomb_000_18_2_4.png");
    feat.nbCircles(i);
    i = openImage("../../data/output/bomb/bomb_002_12_1_3.png");
    feat.nbCircles(i);
//    feat.exportARFF({ FUNCTION_BOOL, FUNCTION_INT, FUNCTION_STRING, FUNCTION_DOUBLE }, "a/path");
}
