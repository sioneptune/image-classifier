#include "Snippet.h"

Snippet::Snippet(const string &scripter, const string &page) : scripter(scripter), page(page) {
    form = scripter + page;
}

ofstream& operator<<(ofstream& ofs, const Snippet d) {
    ofs << "label " << d.getLabel() << endl
    << "form " << d.getForm() << endl
    << "scripter " << d.getScripter() << endl
    << "page " << d.getPage() << endl
    << "row " << d.getRow() << endl
    << "column " << d.getColumn() << endl
    << "size " << d.getSize() << endl;
    return ofs;
}

const string Snippet::getName() const {
    return label + "_" + scripter + "_" + page + "_" + to_string(row) + "_" + to_string(column);
}

const Mat& Snippet::getImg() const {
    return img;
}

void Snippet::setImg(const Mat &img) {
    Snippet::img = img;
}

const string& Snippet::getLabel() const {
    return label;
}

void Snippet::setLabel(const string &label) {
    Snippet::label = label;
}

const string& Snippet::getForm() const {
    return form;
}

const string& Snippet::getScripter() const {
    return scripter;
}

const string& Snippet::getPage() const {
    return page;
}

int Snippet::getRow() const {
    return row;
}

void Snippet::setRow(int row) {
    Snippet::row = row;
}

int Snippet::getColumn() const {
    return column;
}

void Snippet::setColumn(int column) {
    Snippet::column = column;
}

const string& Snippet::getSize() const {
    return size;
}

void Snippet::setSize(const string &size) {
    Snippet::size = size;
}


#include "tools.h"

int __main(int argc, char **argv) {
    Snippet d = Snippet("001" , "02");
    d.setColumn(2);
    d.setRow(1);
    d.setLabel("fire");
    d.setSize("small");
    saveImgDesc("../../../data/imgdesc.txt", d);
}