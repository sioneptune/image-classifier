#include "feature-extraction/FeatureExtractor.h"

FeatureExtractor::~FeatureExtractor() {
    for(auto f: results) {
        delete f;
    }
}

void FeatureExtractor::setImage(const Mat& img){
    image = img;
}

Feature* FeatureExtractor::functionBool() const {
    return new FeatureBool(FUNCTION_BOOL, false);
}

Feature* FeatureExtractor::functionString() const {
    return new FeatureString(FUNCTION_STRING, "{pelican, pingouin, cockatiel}", "cockatiel");
}

Feature* FeatureExtractor::functionInt() const {
    return new FeatureInt(FUNCTION_INT, 124);
}

Feature* FeatureExtractor::functionDouble() const {
    return new FeatureDouble(FUNCTION_DOUBLE, -34.56);
}

void FeatureExtractor::exportARFF(const vector<FeatureFunction> &list, const string inputPath, const string outputPath) {
    // initialization of bouding box attributes
    vector<Point> imgBoundingBox = boundingBox(image);
    upLeftCorner = imgBoundingBox[0];
    downRightCorner = imgBoundingBox[1];

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

                // initialization of image attribute
                setImage(img);

                // Extraction
                for (FeatureFunction f : list) {
                    switch (f) {
                        case FUNCTION_BOOL:     feat = functionBool();
                            break;
                        case FUNCTION_STRING:   feat = functionString();
                            break;
                        case FUNCTION_INT:      feat = functionInt();
                            break;
                        case FUNCTION_DOUBLE:   feat = functionDouble();
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
    FeatureExtractor feat;
    feat.exportARFF({ FUNCTION_BOOL, FUNCTION_INT, FUNCTION_STRING, FUNCTION_DOUBLE }, "../../data/output/", "../../data/");
}
