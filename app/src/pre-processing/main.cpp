#include <iostream>

using namespace std;

#include "opencv2/imgproc.hpp"

using namespace cv;

#include "tools.h"
#include "pre-processing/Snippet.h"
#include "pre-processing/Rectificator.h"
#include "pre-processing/SquareDetector.h"
#include "pre-processing/MatchTemplate.h"

int a_main() {

    // Const
    const string databasePath = "../../data/database/";
    const string outputPath = "../../data/output/";

    // Vars to navigate in the pages
    const int rowSize = 345;

    const int yRow1 = 680;

    const int xStartRow = 160;
    const int xEndRow = 2350;
    const int xPattern = 340;

    MatchTemplate mt("../../data/templates/");

    /// SCRIPTER (folder) => [0-34]
    for (int scripter = 0; scripter<35; scripter++) {
        cout << "--- SCRIPTER: " << scripter << " ---" << endl;

        string scrNb;
        if (scripter < 10) {
            scrNb += "0";
        }
        scrNb += "0" + to_string(scripter);

        /// PAGE => [0-21]
        for (int page = 0; page<22; page++) {
            cout << "page: " << page << endl;

            string pgNb;
            if (page < 10) {
                pgNb += "0";
            }
            pgNb += to_string(page);

            Mat currentPage = openImage(databasePath + scrNb + pgNb + ".png");

            /// Rotation of the page to straighten it
            try {
                Rectificator::rectify(currentPage);
            } catch (CrossNotDetected & cnd) {
                cerr << cnd.what() << " Page " << scrNb << pgNb << " isn't treated." << endl;
                continue;
            }

            /**** WE EXTRACT AND SAVE SNIPPETS PAGE BY PAGE ****/
            vector<Snippet> pageSnippets;

            /// ROW
            for (int row = 0; row<7; row++) {
                int yCurrentRow = yRow1 + row * rowSize;
                int yNextRow = yRow1 + (row+1) * rowSize;
                Mat rowImg = regionOfInterest(currentPage, xStartRow, yCurrentRow, xEndRow, yNextRow);

                /// Pattern detection
                Mat patternImg = regionOfInterest(rowImg, 0, 0, xPattern, rowSize);
                Mat sizeImg = regionOfInterest(patternImg, 0, 230, patternImg.cols, patternImg.rows);

                // Icon size matching
                string labelSize;
                if (mt.hasSizeLabel(sizeImg)) {
                    labelSize = mt.findSize(patternImg);
                }

                // Icon matching
                string label = mt.findSymbol(patternImg);

                /// Squares extraction
                vector<Square> squares;
                SquareDetector::findSquares(rowImg, squares);

                vector<Point> topLefts;
                SquareDetector::extractTopLeftVertices(squares, topLefts);

                /// Snippet extraction
                for (int column = 0; column < topLefts.size(); column++) {
                    Mat drawedIcon = regionOfInterest(rowImg, topLefts[column], 7, 248);

                    Snippet snippet = Snippet(scrNb, pgNb);
                    snippet.setImg(drawedIcon);
                    snippet.setSize(labelSize);
                    snippet.setLabel(label);
                    snippet.setRow(row + 1); // rows start at 1 IRL
                    snippet.setColumn(column + 1); // columns start at 1 IRL

                    pageSnippets.push_back(snippet);
                }
            }

            /// Exporting the snippets of the current page
            for (Snippet d: pageSnippets) {
                // Save the image of the snippet
                string snippetName = outputPath + d.getName();

                saveImg(snippetName + ".png", d.getImg());

                // Save the description of the snippet
                saveImgDesc(snippetName + ".txt", d);
            }
        }
    }

    return EXIT_SUCCESS;
}
