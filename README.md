#Projet Traitement Images et Vidéos

##Cahier des charges
* 1 file READ_ME
Explains the method used + performance results: rate of success, quality of
extraction, cases of failure...
* 1 data directory containing the set of all snippets (mini-images of extracted
drawings) from the base
o For each snippet: 1 .png file + 1 .txt description file

###Files names:
image file: iconeID\_scripterNumber\_pageNumber\_row_column.png
description file: iconeID\_scripterNumber\_pageNumber\_row_column.txt

###Contents of description file:
\# free comment (group name, year...) (other comment lines allowed)
```label <labelName>
form <formNumber=scripterNumberpageNumber>
scripter <scripterNumber>
page <pageNumber>
row <rowNumber
column <columnNumber>
size <small/medium/large (or nothing if size not extracted)>
```

###Example
fire\_000\_02\_1\_2.png
fire\_000\_02\_1_2.txt containing
\# .................
```label fire
form 00002
scripter 000
page 02
row 1
column 2
size medium
```

###Notes
• Do not keep the square border of the drawings
• Keep all information from the original drawing: snippet = sub-images extracted
from non-modified original color images.


##NOTES
Eliminer les images qui se finissent par "22.png"
Se servir des mires pour repositionner l'image
Le binaire ne sert à rien : on utilise le nom de l'image directement
