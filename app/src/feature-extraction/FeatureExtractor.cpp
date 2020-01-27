#include "feature-extraction/FeatureExtractor.h"
void FeatureExtractor::exportARFF(const vector<FeatureFunction> &list, const string inputPath, const string outputPath) {
    Feature *feat = nullptr;
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
                iname = inputPath + iname;
                cout << "Iname : " + iname << endl;
                img = openImage(iname);

                // initialization of image attribute
                setImage(img);

                // initialization of bouding box attributes
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
                            results.insert(results.end(), featureVect.begin(), featureVect.end()) ;
                            break ;
                        case HEIGHT_WIDTH_RATIO:
                            feat = heightWidthRatio();
                            results.push_back(feat);
                            break;
                    }
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
    else cerr << "Unable to open file: " << name << endl;
}

vector<Feature *> FeatureExtractor::barycenter() const {
    vector<Point> nonzero;
    Mat binim;
    cvtColor(image,binim,COLOR_BGR2GRAY);
    threshold(binim, binim,200,255,THRESH_BINARY_INV);
    findNonZero(binim, nonzero);
    int top = 0;
    int bottom = 1000;
    int left = 1000;
    int right = 0;
    Point sum = Point(0, 0);

    for (Point p : nonzero) {
        if (p.x > right) right = p.x;
        if (p.x < left) left = p.x;
        if (p.y > top) top = p.y;
        if (p.y < bottom) bottom = p.y;

        sum += p;
    }

    Point average = Point(sum.x / nonzero.size(), sum.y / nonzero.size());
    Point center = Point((right + left) / 2, (top + bottom) / 2);
    double baryx = (double)(average.x - center.x)/((double)(left - right));
    double baryy = (double)(average.y - center.y)/((double)(top - bottom));

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

int main(){
    FeatureExtractor feat;
    feat.exportARFF({BARYCENTER, HEIGHT_WIDTH_RATIO}, "../../data/output/", "../../data/");
}
