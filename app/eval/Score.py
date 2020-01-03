import sys
import os
import csv

# VAR Globales
SAMPLE_ROWS = 270
SAMPLE_SIZE = SAMPLE_ROWS * 5

NB_SCRIPTERS = 35
NB_PAGES_PER_SET = 22

NB_ROW_PER_PAGE = 7
NB_COLUMNS_PER_PAGE = 5

NB_PICTURES = NB_SCRIPTERS * NB_PAGES_PER_SET * NB_ROW_PER_PAGE * NB_COLUMNS_PER_PAGE

def loadCSV(path):
    reader = csv.DictReader(open(path, mode='r'))
    theoricSampleSet = []

    for line in reader:
        # Format the row descriptor
        scripter = int(line['Scripter'])
        page = int(line['Page'])
        row = int(line['Row'])
        icon = str.lower(line['Icon'])
        size = str.lower(line['Size'])

        theoricSampleSet.append({"Scripter":scripter,"Page":page,"Row":row,"Icon":icon,"Size":size})

    return theoricSampleSet

def openDescriptionFile(path):
    ''' Return a dictionnary from the file descriptor txt. Keys are similar to csv '''
    lineCounter = 0

    try:
        with open(path,mode='r') as txt:
            # Remove comment lines
            lines = [x for x in txt.readlines() if x[0]!='#']
            #print('File detected at :'+path)
        # Format the row descriptor
        scripter = int(lines[lineCounter+2].split(' ')[1])
        page = int(lines[lineCounter+3].split(' ')[1]) # Get string describing the page and convert to int
        row = int(lines[lineCounter+4].split(' ')[1])
        icon = str.lower((lines[lineCounter].split(' ', 1)[1]).replace('\n','')) #Split on first occurence (for "fire brigade") and remove \n
        size = str.lower((lines[lineCounter+6].split(' ')[1]).replace('\n',''))
   
    except FileNotFoundError:
        print('Exception for path :'+path)
        scripter = -1
        page = -1
        row = -1
        icon = 'none'
        size = 'none'

    fileDescriptor = {"Scripter":scripter,"Page":page,"Row":row,"Icon":icon,"Size":size}
    return fileDescriptor


def score(pathCSV, pathDB):
    ''' Simple computation of the score of correct labelling of the file descriptors in the Picture Database ''' 
    # Scoring
    iconScore = 0
    sizeScore = 0

    theoricSampleSet = loadCSV(pathCSV)
    files = os.listdir(pathDB)

    for sample in theoricSampleSet:
        #Scripter formatting
        scripterTh = sample['Scripter']
        if scripterTh < 10 :    scripter = '00'+str(scripterTh)
        elif scripterTh < 100:  scripter = '0'+str(scripterTh)
        else:                   scripter = str(scripterTh)
        
        #Page formatting
        pageTh = sample['Page']
        if pageTh < 10 :    page = '0'+str(pageTh)
        else :              page = str(pageTh)

        row = str(sample['Row'])
        
        for c in range(0,NB_COLUMNS_PER_PAGE+1):
            #Check matching file(s)
            identifier = '_'+scripter+'_'+page+'_'+row+'_'+str(c)+'.txt'
            # print(identifier)
            
            matchingFileName = [name for name in files if name.endswith(identifier)]
            #Open matching file to get the fileDescriptor
            
            for file in matchingFileName:
                fileDescriptor = openDescriptionFile(pathDB+file)

                if fileDescriptor['Icon'] == sample['Icon']:
                    iconScore += 1
                else: 
                    print('##### ICON ALERT #####')
                    print(fileDescriptor)
                    print('Theoric Model :' + str(sample))
                if fileDescriptor['Size'] == sample['Size']:
                    sizeScore += 1
                else: 
                    print('##### SIZE ALERT #####')
                    print(fileDescriptor)
                    print('Theoric Model :' + str(sample))

    print('Scores based on a theoric random sample set of {0} icons over {1} images in total, representing {2:.3g}% of the set'.format(SAMPLE_SIZE, NB_PICTURES, (100*SAMPLE_SIZE)/NB_PICTURES ))
    print('Correct guessed icons: (Sample Set) {0} / {1}, Correctness: {2:.3g}%'.format(iconScore, SAMPLE_SIZE, (iconScore*100)/SAMPLE_SIZE))
    print('                       (Total Set) Correctness: {0:.3g}% of {1:.3g}% evaluated'.format((iconScore*100)/NB_PICTURES, (SAMPLE_SIZE*100)/NB_PICTURES))
    print('Correct guessed sizes: (Sample Set) {0} / {1}, Correctness: {2:.3g}%'.format(sizeScore, SAMPLE_SIZE, (sizeScore*100)/SAMPLE_SIZE))
    print('                       (Total Set) Correctness: {0:.3g}% of {1:.3g}% evaluated'.format((sizeScore*100)/NB_PICTURES, (SAMPLE_SIZE*100)/NB_PICTURES))


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

