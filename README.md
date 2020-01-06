# Image classifier project

Authors: Matthieu DARFAY, Théotime DUGOIS, Zoé LEVREL, Mathieu RISY.
The project is made in C++, using the OpenCV library. The evaluation of the results is done using a Python script.

## Goal of the project
The main purpose of the project is to make a pattern recognition application.
The project is based around 3 steps :
- Pre-processing and image processing
- Feature extraction
- Classification

At each step, a the results are evaluated to check the quality of the work done during the step.

The goal is to recognize a set of 14 icons used by emergency services. The icons were hand drawn by 35 volunters, called scripters, making up the Niclcon database. The dataset used in this project is composed of sheets extracted from this database.

## Data representation
Each icon drawn by a scripter will be called a **Snippet**.
For each snippet, 2 files are created:
- 1 *.png* file, corresponding to the extracted image
- 1 *.txt* description file

### Files names
**Image file:** *iconID_scripterNumber_pageNumber_row_column.png*
**Description file:** *iconID_scripterNumber_pageNumber_row_column.txt*

### Contents of description file
```
label <labelName>
form <formNumber=scripterNumberpageNumber>
scripter <scripterNumber>
page <pageNumber>
row <rowNumber>
column <columnNumber>
size <small/medium/large (or nothing if size not extracted)>
```

**Example:**
*fire\_000\_02\_1\_2.png*
*fire\_000\_02\_1_2.txt* containing
```
label fire
form 00002
scripter 000
page 02
row 1
column 2
size medium
```

## Launch
Two `main` are written in the project because the training database and the test one don't have the same name format. Be sure to have only one `main` by changing the name of the one you don't want to run.


### Training database
Prerequisites:
* Put images of the training database in the folder *image-classifier/data/database*
* Create a folder *output* in *image-classifier/data/*

Run `main.cpp`

### Test database
Prerequisites:
* Put images of the training database in the folder *image-classifier/data/database_test*
* Create a folder *output_test* in *image-classifier/data/*

Run `main_test.cpp`

### Evaluating the results

Navigate to app/eval. The evaluation script (written in Python) can be run in one of two ways:
* To evaluate the training dataset: ```python3 Score.py path_to_csv path_to_output_folder```. With our current file structure that command works out to: `python3 Score.py ./Theoric_Dataset.csv ../../data/output`
* To evaluate both the training set and the dataset: Run the same command, but add the paths to the test csv and test output folder. For us: `python3 Score.py ./Theoric_Dataset.csv ../../data/output Theoric_Dataset_Test.csv ../../data/output_test`

If you enter the wrong number of arguments, the script should helpfully notify you of your mistake.

## Pre-processing
**Specifications:**
* Do not keep the square frame of the drawings
* Keep all information from the original drawing: snippet = sub-images extracted
from non-modified original color images.

### Steps of the pre-processing
**Observation:**
The 22nd page of each scripter is not relevant for our project because it only consist in a full page of handwritten text.

#### Step 1) Identify the cross targets and rectify the picture
We detect the 2 crosses located in the upper right and lower left of each page, and use them to check if the page has been rotated.

We want the page to be straight and appropriately scaled: to ensure this we compute the angle between the horizontal axis and the line between the 2 crosses, and rotate the image the appropriate amount to straighten it, as well as scaling it the relevant amount. Once the image has been straightened in this way, we detect a cross again and use it to shift the image into position. In this way, every image is straight, properly scaled and properly positioned.

Method used to detect the crosses:
1. Select regions where we expect the crosses to be, even for heavily rotated images.
2. Apply erosions to keep only pixels of the center of the cross.
	=> One erosion with a cross structuring element of size 5px*5px
	=> One erosion with a circle structuring element of size 3px*3px
3. Calculate the average position of all remaining pixels, to extrapolate the expected center of the cross.

We then compute the tangent between the centers of the crosses, and compare it to a reference angle, which we measured on a manually straightened image. We use the difference between these angles to determine the rotation to apply to the picture in order to straighten it.

If we can't detect one of the crosses on the image, we have decided to not process it. There are many reasons it could be undetected (image extremely rotated, excessively cropped, low quality, ...). The time and processing power needed to try and detect those problems and fix them is not justified given that only a very small number of pages do not get their crosses detected. From empirical observations, only one image has an undetectable cross inside the given training dataset. Therefore we assume that ignoring pages with undetectable crosses would not have a noticeable impact on our results.

#### Step 2) Detect the squares and extract the snippets
We process each page line by line.
For each line, squares are detected, and sub-images are extracted from inside these squares.
This step is inspired by the OpenCV example avalaible at this [adress](https://github.com/opencv/opencv/blob/master/samples/cpp/squares.cpp).

The algorithm works as follows:
1. Process the image to remove noise (using `pyrDown` and `pyrUp` from OpenCV).
2. Detect contours in the picture (using `findContours`). We keep only the external contours, as they are enough to surround the squares.
3. Filter non-quadrilateral contours, in order to remove noisy contours and keep only squares.
4. Select only interesting contours: we remove the ones that are too small or too large, based on the expected area of the squares.

Then, for each square, we determine its top-left corner point and extract a snippet from this point. In order to not have the border on the snippet, we add an offset to the top-left point, to extract the image from inside the square.
Each snippet image is a square image, of equal and fixed dimensions.

#### Step 3) Identify the icon and the size of the snippet
We use the `matchTemplate` function from OpenCV.

We have 2 sets of images:
- all the 14 possible icons,
- and images of the 3 different texts "small", "medium", "large".
These reference images are used as templates (see [/data/templates/]), that we try to find inside the pages.

*Note:*
*The first 2 pages for each scripter don't have the size written under the icons. Therefore we do not try to match the sizes templates on these.*

For each line, we extract the zone where the icon and its eventual size are.
All the templates are tested on the zone, and the best matching one is associated with the current line and its snippets.

### Results of the pre-processing
**Global results:**
The ground-truth was established for the two databases. For the training database, 270/1350 lines (5.01%) were annotated. For the test database, all the 84 lines (100%) were annotated.

On each database, 100% of the annotated icons and sizes were correctly detected by our application.

On each database, no outlines are visible on the annotated Snippets (constituing the ground-truth).

**Results on training database:**
Over the 26950 snippets that it's possible to extract, our pre-processing successfully extracted 26728. We haven't got any Snippet extracted twice (concluded from tests made during debug in the pre-processing process, when processing the whole training database). 

99,18% of the training database was correctly extracted, which is pretty satisfying. We could extract more Snippets without the shifting of the page, that induces somes fails in the squares detection. Nevertheless, shifting the page improves the robustness of our application, and is a reasonable reason to lose a few Snippets in our opinion.

From empirical observations, we have been able to see that at least 3 lines weren't associated with the right icon (we didn't see other errors just by watching the images). 
These lines are:
- line 4, page 02, scripter 13,
- line 4, page 02, scripter 28,
- line 3, page 01, scripter 34.

The theoritical icons are 'Car' but 'Paramedics' was matched instead. Since it represents only 15 Snippets over the 26728 extracted, we didn't change our icon detection method. It would have taken us too much time to change it, with risks to overfit the detection to the training dataset.
