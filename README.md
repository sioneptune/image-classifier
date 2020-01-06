# Image classifier project

Authors: Matthieu DARFAY, Théotime DUGOIS, Zoé LEVREL, Mathieu RISY.
The project is made in C++, with the OpenCV library (main application) and some of the evaluations are made in Python.

## Goal of the project
The main purpose of the project is to realize a pattern recognition application.
The project is composed of 3 steps :
- Pre-processing and image processing
- Feature extraction
- Classification

At each step, a phase of evaluation is made to verify the quality of the work done during the step.

The objective is to recognize a set of 14 icons used by emergency services. The icons were hand drawn by 35 scripters, constituing the Niclcon database. The data used in this project are extracted from this database.

## Data representation
Each icon drawn by a scripter will be called a **Snippet**.
For each snippet, 2 files are created:
- 1 *.png* file, corresponding to the extracted image
- 1 *.txt* description file

### Files names
**Image file:** *iconeID_scripterNumber_pageNumber_row_column.png*
**Description file:** *iconeID_scripterNumber_pageNumber_row_column.txt*

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

## Pre-processing
**Specifications:**
* Do not keep the square border of the drawings
* Keep all information from the original drawing: snippet = sub-images extracted
from non-modified original color images.

### Steps of the pre-processing
**Observation:**
The 22th page of each scripter is useless for our project because it only consist in a full page of handwritten text.

#### Step 1) Identify the cross targets and straighten the picture
We detect the 2 crosses situated in the corners of the page, and use them to straighten the image if necessary. 

We want the page to be straight: we calculate the angle between the horizontal axis and the line between the 2 crosses to determine if a rotation is necessary.

Method used to detect the crosses:
1. Select regions where the crosses are.
2. Apply erosions to keep only pixels of the center of the cross.
	=> One erosion with a cross structuring element of size 5px*5px
	=> One erosion with a circle structuring element of size 3px*3px
3. Calculate the average position of all remaining pixels, to represent the center of the cross.


We then calculate the tangent between the centers of the crosses, and compare it to a reference angle. We use the difference between these angles to determine the rotation to apply to the picture in order to straighten it.

If we can't detect one of the crosses on the image, we have decided to not process it, because it brings too many theoretical problems (image extremely rotated, too much cropped, bad quality, ...). From empirical observations, only one image has an undetectable cross inside the given training dataset. 
Then our decision to ignore pictures with undetectable crosses seems to be reasonable and not processing a few pages shouldn't affect too much our final application.

#### Step 2) Detect the squares and extract the snippets
We process each page line by line.
For each line, squares are detected, and then sub-images are extracted inside these squares.
This step is inspired from the OpenCV example avalaible here: [https://github.com/opencv/opencv/blob/master/samples/cpp/squares.cpp].

The method is the following:
1. Process the image to remove noise (using `pyrDown` and `pyrUp` from OpenCV).
2. Detecting contours in the picture (using `findContours`). We keep only external ones, because it is enough to surround the squares.
3. Filter non-quadrilateral contours, in order to remove noisy contours and keep only squares.
4. Select only interesting contours: we remove the ones being too small or too large, based on the expected area of the squares.

Then, for each square, we get its top-left corner point and extract a snippet from this point. In order to not have the border on the snippet, we add an offset to the top-left point, to extract the image from inside the square.
Each snippet image is a square image, of equal and fixed lenght.

#### Step 3) Identify the icon and the size of the snippet
Use of the `matchTemplate` function from OpenCV.

We have 2 sets of images:
- all the 14 possible icons,
- and images of the 3 different texts "small", "medium", "large".
These reference images are used as templates (see [/data/templates/]), that we try to find inside the pages.

*Note:*
*The first 2 pages for each scripter don't have the size written under the icons. We just don't try to match the sizes templates on these.*

For each line, we extract the zone where the icon and its eventual size are.
All the templates are tested on the zone, and the best matching one is associated with the current line and its snippets.

### Results of the pre-processing




