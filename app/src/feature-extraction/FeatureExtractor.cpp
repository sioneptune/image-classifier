#include "feature-extraction/FeatureExtractor.h"

FeatureExtractor::FeatureExtractor() {
    results = vector<Feature*>();
}

Feature* FeatureExtractor::functionBool() {
    return new FeatureBool(FUNCTION_BOOL, false);
}

Feature* FeatureExtractor::functionString() {
    return new FeatureString(FUNCTION_STRING, "{pelican, pingouin, cockatiel}", "cockatiel");
}

Feature* FeatureExtractor::functionInt() {
    return new FeatureInt(FUNCTION_INT, 124);
}

Feature* FeatureExtractor::functionDouble() {
    return new FeatureDouble(FUNCTION_DOUBLE, -34.56);
}

void FeatureExtractor::exportARFF(const vector<FeatureFunction> &list, string path) {
    Feature *feat = nullptr;
    cout << "Exportation to " + path << endl;
    // TODO : Export the Title in the final file
    cout << "@RELATION EXTRACTED_IMAGES\n" << endl;

    // Extraction
    for(FeatureFunction f : list) {
        switch (f) {
            case FUNCTION_BOOL: feat = functionBool();
                break;
            case FUNCTION_STRING: feat = functionString();
                break;
            case FUNCTION_INT: feat = functionInt();
                break;
            case FUNCTION_DOUBLE: feat = functionDouble();
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
    for(Feature* f : results) {
        tinyRes+= f->getValue()+",";
    }
    cout << tinyRes << endl;
}

int a_main(){
    FeatureExtractor feat = FeatureExtractor();
    feat.exportARFF({ FUNCTION_BOOL, FUNCTION_INT, FUNCTION_STRING, FUNCTION_DOUBLE }, "a/path");
}
