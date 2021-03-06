#ifndef IMAGE_CLASSIFIER_SNIPPET_H
#define IMAGE_CLASSIFIER_SNIPPET_H

#include <string>
#include <fstream>

using namespace std;

#include "opencv2/imgproc.hpp"

using namespace cv;

class Snippet {

private:
    Mat img;
    string label;
    string form;
    string scripter;
    string page;
    int row;
    int column;
    string size;

public:
    Snippet(const string &scripter, const string &page);

    const string getName() const;

    const Mat& getImg() const;
    void setImg(const Mat &img);

    const string& getLabel() const;
    void setLabel(const string &label);

    const string& getForm() const;

    const string& getScripter() const;

    const string& getPage() const;

    int getRow() const;
    void setRow(int row);

    int getColumn() const;
    void setColumn(int column);

    const string& getSize() const;
    void setSize(const string &size);

};

ofstream& operator<<(ofstream& ofs, const Snippet d);


#endif //IMAGE_CLASSIFIER_SNIPPET_H
