#include "Drawing.h"

Drawing::Drawing(const string &form) : form(form) {
    scripter = form.substr(0, 3);
    page = form.substr(3, 2);
}

ofstream& operator<<(ofstream& ofs, const Drawing d) {
    ofs << "label " << d.getLabel() << endl
    << "form " << d.getForm() << endl
    << "scripter " << d.getScripter() << endl
    << "page " << d.getPage() << endl
    << "row " << d.getRow() << endl
    << "column " << d.getColumn() << endl
    << "size " << d.getSize() << endl;
    return ofs;
}


const Mat& Drawing::getImg() const {
    return img;
}

void Drawing::setImg(const Mat &img) {
    Drawing::img = img;
}

const string& Drawing::getLabel() const {
    return label;
}

void Drawing::setLabel(const string &label) {
    Drawing::label = label;
}

const string& Drawing::getForm() const {
    return form;
}

const string& Drawing::getScripter() const {
    return scripter;
}

const string& Drawing::getPage() const {
    return page;
}

int Drawing::getRow() const {
    return row;
}

void Drawing::setRow(int row) {
    Drawing::row = row;
}

int Drawing::getColumn() const {
    return column;
}

void Drawing::setColumn(int column) {
    Drawing::column = column;
}

const string& Drawing::getSize() const {
    return size;
}

void Drawing::setSize(const string &size) {
    Drawing::size = size;
}


#include "tools.h"

int _main(int argc, char **argv) {
    Drawing d = Drawing("00102");
    d.setColumn(2);
    d.setRow(1);
    d.setLabel("fire");
    d.setSize("small");
    saveImgDesc("../../../data/imgdesc.txt", d);
}