#include "feature-extraction/FeatureExtractor.h"

FeatureExtractor::~FeatureExtractor() {
    for(auto f: results) {
        delete f;
    }
}

void FeatureExtractor::setImage(const Mat& img){
    image = img;
}

Feature* FeatureExtractor::heightWidthRatio() const {
    int height = downRightCorner.y - upLeftCorner. y;
    int width = downRightCorner.x - upLeftCorner.x;
    return new FeatureDouble(HEIGHT_WIDTH_RATIO,1.0 * height / width );
}

void FeatureExtractor::exportARFF(const vector<FeatureFunction> &list, const string inputPath, const string outputPath) {
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

                // initialization of bouding box attributes
                vector<Point> imgBoundingBox = boundingBox(image);
                upLeftCorner = imgBoundingBox[0];
                downRightCorner = imgBoundingBox[1];

                // Extraction
                for (FeatureFunction f : list) {
                    switch (f) {
                        case HEIGHT_WIDTH_RATIO:     feat = heightWidthRatio();
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
    feat.exportARFF({ HEIGHT_WIDTH_RATIO}, "../../data/output_extract/", "../../data/output_extract/");
}
