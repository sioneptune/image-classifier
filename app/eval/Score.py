import sys
import os
import csv

# VAR Globales
NB_SCRIPTERS = 35
NB_PAGES_PER_SET = 22

NB_ROW_PER_PAGE = 7
NB_COLUMNS_PER_PAGE = 5

NB_PICTURES = NB_SCRIPTERS * NB_PAGES_PER_SET * NB_ROW_PER_PAGE * NB_COLUMNS_PER_PAGE

def loadCSV(path):
    reader = csv.DictReader(open(path, mode='r'))
    dictMatrix = [None] * NB_PAGES_PER_SET

    for i in range(NB_PAGES_PER_SET):
        dictMatrix[i] = [None] * NB_ROW_PER_PAGE

    for row in reader:
        # Format the row descriptor
        pageNb = int(row['Page'])
        rowNb = int(row['Row'])
        row['Page'] = pageNb
        row['Row'] = rowNb
        row['Icon'] = str.lower(row['Icon'])
        row['Size'] = str.lower(row['Size'])

        dictMatrix[pageNb][rowNb -1] = row

    return dictMatrix

def openDescriptionFile(path):
    ''' Return a dictionnary from the file descriptor txt. Keys are similar to csv '''
    lineCounter = 0

    try:
        with open(path,mode='r') as txt:
            # Remove comment lines
            lines = [x for x in txt.readlines() if x[0]!='#']
            print('File detected at :'+path)
        # Format the row descriptor
        page = int(lines[lineCounter+3].split(' ')[1]) # Get string describing the page and convert to int
        row = int(lines[lineCounter+4].split(' ')[1])
        icon = str.lower((lines[lineCounter].split(' ')[1]).replace('\n','')) # Get string and remove \n
        size = str.lower((lines[lineCounter+6].split(' ')[1]).replace('\n',''))
   
    except FileNotFoundError:
        print('Exception for path :'+path)
        page = -1
        row = -1
        icon = 'none'
        size = 'none'

    fileDescriptor = {"Page":page,"Row":row,"Icon":icon,"Size":size}
    return fileDescriptor


def score(pathCSV, pathDB):
    ''' Simple computation of the score of correct labelling of the file descriptors in the Picture Database ''' 

    theoricDatabase = loadCSV(pathCSV)
    files = os.listdir(pathDB)

    # Scoring
    iconScore = 0
    sizeScore = 0
    for scr in range(0,NB_SCRIPTERS):                      #For each scripter
        if scr < 10 :   scripter = '00'+str(scr)
        elif scr < 100: scripter = '0'+str(scr)
        else :          scripter = str(scr)
            
        for p in range(NB_PAGES_PER_SET):                   #For each page
            if p < 10 : page = '0'+str(p)
            else :      page = str(p)

            for r in range(1,NB_ROW_PER_PAGE+1):               #For each row
                for c in range(1,NB_COLUMNS_PER_PAGE+1):       #For each column
                    #Check matching file(s)
                    identifier = '_'+str(scripter)+'_'+str(page)+'_'+str(r)+'_'+str(c)+'.txt'
                    # print(identifier)
                    matchingFileName = [name for name in files if name.endswith(identifier)]
                    #Open matching file to get the fileDescriptor
                    for file in matchingFileName:
                        fileDescriptor = openDescriptionFile(pathDB+file)
                        print(fileDescriptor)
                        print('Theoric Model :')
                        print(theoricDatabase[p][r-1])
                        
                        if fileDescriptor['Icon'] == theoricDatabase[p][r-1]['Icon']:
                            iconScore += 1
                        if fileDescriptor['Size'] == theoricDatabase[p][r-1]['Size']:
                            sizeScore += 1

    print('The score of correct guessed icons is : {0} [{1} correct over {2}]'.format(iconScore/NB_PICTURES, iconScore, NB_PICTURES))
    print('The score of correct guessed sizes is : {0} [{1} correct over {2}]'.format(sizeScore/NB_PICTURES, sizeScore, NB_PICTURES))


if __name__ == "__main__":
    # count the arguments
    if ( len(sys.argv) - 1 != 2 ):
        print("Invalid number of argument !\nYou should call the script as : Score.py \'path/to/csv\' \'path/to/databaseFolder\'\nThank you !")
        exit()

    pathCSV = (sys.argv[1]).replace('\'','') #Get path to folder without quotes
    pathDB = (sys.argv[2]).replace('\'','') #Get path to folder without quotes
    # Check path format
    if (pathDB[len(pathDB)-1] != '/'):
        pathDB += "/"
    
    score(pathCSV, pathDB)

