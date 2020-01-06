#include <string>
#include <MatchTemplate.h>
#include <tools.h>
#include <Straightener.h>
#include <SquareDetector.h>

using namespace std;


int _main() {
    // Const
    const string databasePath = "../../data/database_test/";
    const string outputPath = "../../data/output_test/";

    // Vars to navigate in the pages
    const int rowSize = 345;

    const int yRow1 = 680;

    const int xStartRow = 160;
    const int xEndRow = 2350;
    const int xPattern = 340;

    MatchTemplate mp("../../data/templates/");

    /// SCRIPTER (folder) => [1-6]
    for (int scripter = 1; scripter<=6; scripter++) {
        cout << "--- SCRIPTER: " << scripter << " ---" << endl;

        string scrNb = "s0" + to_string(scripter);

        /// PAGE => [1-2]
        for (int page = 1; page<=2; page++) {
            cout << "page: " << page << endl;

            string pgNb = "000" + to_string(page);

            Mat currentPage = openImage(databasePath + scrNb + "_" + pgNb + ".png");

            /// Rotation of the page to straighten it
            try {
                Straightener::straighten(currentPage);
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

                if (mp.hasSizeLabel(sizeImg)) {
                    labelSize = mp.findSize(patternImg);
                }

                // Icon matching
                string label = mp.findSymbol(patternImg);

                /// Squares extraction
                vector<Square> squares;
                SquareDetector::findSquares(rowImg, squares);

                vector<Point> topLefts;
                SquareDetector::extractTopLeftVertices(squares, topLefts);

                /// Snippet extraction
                for (int column = 0; column < topLefts.size(); column++) {
                    Mat drawedIcon = regionOfInterest(rowImg, topLefts[column], 7, 248);

                    string scrNbExport;
                    if ( scripter < 10 )
                        scrNbExport = "00" + to_string(scripter);
                    else if (scripter < 100)
                        scrNbExport = "0" + to_string(scripter);
                    else
                        scrNbExport = to_string(scripter);

                    string pgNbExport;
                    if (page < 10 )
                        pgNbExport = "0" + to_string(page);
                    else
                        pgNbExport = to_string(page);


                    Snippet snippet = Snippet(scrNbExport, pgNbExport);
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

