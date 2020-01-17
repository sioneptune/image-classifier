#include "feature-extraction/FeatureExtractor.h"

FeatureExtractor::FeatureExtractor() {
    results = vector<Feature *>();
}

Feature *FeatureExtractor::functionBool() {
    return new FeatureBool(FUNCTION_BOOL, false);
}

Feature *FeatureExtractor::functionString() {
    return new FeatureString(FUNCTION_STRING, "{pelican, pingouin, cockatiel}", "cockatiel");
}

Feature *FeatureExtractor::functionInt() {
    return new FeatureInt(FUNCTION_INT, 124);
}

Feature *FeatureExtractor::functionDouble() {
    return new FeatureDouble(FUNCTION_DOUBLE, -34.56);
}

void FeatureExtractor::exportARFF(const vector<FeatureFunction> &list, string path) {
    Feature *feat = nullptr;
    cout << "Exportation to " + path << endl;
    // TODO : Export the Title in the final file
    cout << "@RELATION EXTRACTED_IMAGES\n" << endl;

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
        // TODO : Export the Header in the final file
        cout << feat->getDescriptor() << endl;
        results.push_back(feat);
    }

    // Exportation
    // TODO : Export the Data in the final file
    cout << "\n@DATA" << endl;
    string tinyRes = "";
    for (Feature *f : results) {
        tinyRes += f->getValue() + ",";
    }
    cout << tinyRes << endl;
}

vector<Feature *> FeatureExtractor::barycentre(Mat &image) {
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
    Point barycenter = Point(average.x - center.x, average.y - center.y);
    cout << average << endl;
    cout << center << endl;
    FeatureInt baryX = FeatureInt(BARYCENTER_X, barycenter.x);
    FeatureInt baryY = FeatureInt(BARYCENTER_Y, barycenter.y);

    vector<Feature*> res = {&baryX, &baryY};

    return res;
}


int main() {
    FeatureExtractor feat = FeatureExtractor();
    feat.exportARFF({FUNCTION_BOOL, FUNCTION_INT, FUNCTION_STRING, FUNCTION_DOUBLE}, "a/path");
    Mat img = openImage("../../data/output/accident/accident_000_00_1_1.png");
    vector<Feature * > f = feat.barycentre(img);

}
