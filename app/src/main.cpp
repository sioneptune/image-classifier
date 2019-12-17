#include <iostream>

using namespace std;

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

using namespace cv;

#include "tools.h"
#include "Drawing.h"
#include "square_detector.h"
#include "match_pattern.h"

int main(void) {

    // Const
    const string databasePath = "../../data/database/";
    const string outputPath = "../../data/output/";

    // Vars to navigate in the pages
    const int rowSize = 345;

    const int yRow1 = 680;

    const int xStartRow = 160;
    const int xEndRow = 2350;
    const int xPattern = 340;

    MatchPattern mp;

    Timer timer;
    timer.set_start();

    /// SCRIPTER (folder) => [0-34]
    for (int scripter = 0; scripter<35; scripter++) {
        string scrNb;
        if (scripter < 10) {
            scrNb += "0";
        }
        scrNb += "0" + to_string(scripter);

        /// PAGE => [0-21]
        for (int page = 0; page<22; page++) {
            string pgNb;
            if (page < 10) {
                pgNb += "0";
            }
            pgNb += to_string(page);

            Mat currentPage = openImage(databasePath + scrNb + pgNb + ".png");

            /**** WE EXTRACT AND SAVE DRAWINGS PAGE BY PAGE ****/
            vector<Drawing> pageDrawings;

            /// ROW
            for (int row = 0; row<7; row++) {
                int yCurrentRow = yRow1 + row * rowSize;
                int yNextRow = yRow1 + (row+1) * rowSize;
                Mat rowImg = regionOfInterest(currentPage, xStartRow, yCurrentRow, xEndRow, yNextRow);

                /// Pattern detection
                Mat patternImg = regionOfInterest(rowImg, 0, 0, xPattern, rowSize);
                // Icon size matching
                string labelSize;
                if (page > 1) { // Size isn't written under patterns in the first 2 pages
                    labelSize = mp.findSize(patternImg);
                }

                // Icon matching
                string label = mp.findSymbol(patternImg);

                /// Squares extraction
                vector<Square> squares;
                SquareDetector::findSquares(rowImg, squares);

                vector<Point> topLefts;
                SquareDetector::extractTopLeftVertices(squares, topLefts);

                /// Drawing extraction
                for (int column = 0; column < topLefts.size(); column++) {
                    Mat drawedIcon = regionOfInterest(rowImg, topLefts[column], ROI_TL_OFFSET, ROI_SIZE_SQUARE);

                    Drawing drawing = Drawing(scrNb, pgNb);
                    drawing.setImg(drawedIcon);
                    drawing.setSize(labelSize);
                    drawing.setLabel(label);
                    drawing.setRow(row + 1); // rows start at 1 IRL
                    drawing.setColumn(column + 1); // columns start at 1 IRL

                    pageDrawings.push_back(drawing);
                }
            }

            /// Exporting the drawings of the current page
            for (Drawing d: pageDrawings) {
                // Save the image of the drawing
                string drawingName = outputPath + d.getName();
                saveImg(drawingName + ".png", d.getImg());

                // Save the description of the drawing
                saveImgDesc(drawingName + ".txt", d);
            }

        }
    }

    cout << "time: " << timer.set_end().time_elapsed() << endl;



    return EXIT_SUCCESS;
}
