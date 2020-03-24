import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import argparse
import csv
 
#---------GLOBAL VARIABLES----------GLOBAL VARIABLES----------GLOBAL VARIABLES----------GLOBAL VARIABLES

# We need to grab the three arguments from the bash script or user. These arguments in order are [the name of the source folder of the fitness scores], [the name of the destination folder for the plots], and [the number of generations]
parser = argparse.ArgumentParser()
parser.add_argument("source", help="Name of source folder from home directory", type=str)
parser.add_argument("destination", help="Name of destination folder from home directory (no end dash)", type=str)
parser.add_argument("NPOP", help="Number of individuals per generation", type=int)
parser.add_argument("numGens", help="Number of generations the code is running for (no end dash)", type=int)
g = parser.parse_args()

# The name of the plot that will be put into the destination folder, g.destination
Plot2DName = "/FScorePlot2D.png"
Plot3DName = "/FScorePlot3D.png"

#----------STARTS HERE----------STARTS HERE----------STARTS HERE----------STARTS HERE
fileReadTemp = []
fScoresGen = []
fScoresInd = []
"""
#We may want to only have g.numGens and not g.numGens+1
for gen in range(g.numGens+1):
    filename = "/{}_fitnessScores.csv".format(gen)
    #fileReadTemp = np.genfromtxt(g.source + filename, delimiter=',')
    #The above comment was here orignally but the delimiter is no longer a comma so the next line should work with new line
    #fileReadTemp = (line.rstrip('\n') for line in open(g.source + filename))
    #fScoresGen.append(fileReadTemp[2:])
    with open(g.source + filename, "r") as f:
        fileReadTemp = [line.strip() for line in f]
    fScoresGen.append(fileReadTemp[2:])
fScoresInd = np.transpose(fScoresGen)
NPOP = len(fScoresInd)
"""

#new way (similar to in VeffPlotting.py)

tempFitnesses = []
FitnessesArray = []
for ind in range(1, g.NPOP+1):
    lineNum = ind + 1 #the line in the csv files that the individual data is in 
    #we need to loop over all the generations, since the gen is in the file names
    for gen in range(0, g.numGens+1):
        #we need to give the changeable filenames we're gonna read
        fitnesses = "{}_fitnessScores.csv".format(gen)
        #for each generation, we need to get all the fitnesses
        with open(g.source + "/" + fitnesses, "r") as fr: #fr for fitnesses read
            f_read = csv.reader(fr, delimiter=',') #reading fr as a csv
            for i, row in enumerate(f_read): #loop over the rows 
                if i == lineNum: #skipping the header
                    fitness = float(row[0]) #lineNum contains the fitness score
                    print(fitness)
        fr.close()
        #fill the generation individual values into arrays to hold them temporarily
        tempFitnesses.append(fitness)
    #The temporary files contain the same individual at different generations
    #we want to store these now in the arrays containing all the data
    FitnessesArray.append(tempFitnesses)
    tempFitnesses = []
    


genAxis = np.linspace(0,g.numGens,g.numGens+1,endpoint=True)

Veff_ARA = []
Err_plus_ARA = []
Err_minus_ARA = []
Veff_ARA_Ref = []

filenameActual = "/AraOut_ActualBicone.txt"
fpActual = open(g.source + filenameActual)

for line in fpActual:
    if "test Veff(ice) : " in line:
        Veff_ARA = float(line.split()[5]) #changed from 3 to 5 for switching to km^3 from m^3
        #print(Veff_ARA)
    elif "And Veff(water eq.) error plus :" in line:
        Err_plus_ARA = float(line.split()[6])
        Err_minus_ARA = float(line.split()[11])
#    line = fpActual.readline()
    #print(line)

Veff_ARA_Ref = Veff_ARA * np.ones(len(genAxis))

plt.figure(figsize=(10, 8))
plt.plot(genAxis, Veff_ARA_Ref, label = "ARA Reference", linestyle= '--', color = 'k')
for ind in range(g.NPOP):
    LabelName = "Individual {}".format(ind+1)
   
    plt.plot(genAxis, FitnessesArray[ind], label = LabelName, marker = 'o', linestyle='', markersize = 11)

plt.xlabel('Generation', size = 22)
plt.ylabel('Fitness Score', size = 22)
plt.title("Fitness Score over Generations (0 - {})".format(int(g.numGens)), size = 25)
plt.legend()
plt.savefig(g.destination + Plot2DName)

#for x in range (len(fScoresInd[1])):
    #print fScoresInd[1][x]

#plt.show()
# was commented out to prevent graph from popping up and block=False replaced it along with plt.pause
# the pause functions for how many seconds to wait until it closes graph
plt.show(block=False)
plt.pause(15)

plt.figure(figsize=(10, 8))
indAxis = np.linspace(1,g.NPOP,g.NPOP)
genAxis, indAxis = np.meshgrid(genAxis, indAxis)
ax = plt.axes(projection='3d')
#ax.plot_surface(genAxis, indAxis, fScoresInd, rstride=1, cstride=1, cmap='viridis', edgecolor='none')
ax.set_title('3D Fitness Score over Generations');
ax.set_xlabel('Generation')
ax.set_ylabel('Individual')
ax.set_zlabel('Fitness Score')
plt.savefig(g.destination + Plot3DName)
#plt.show()
# was commented out to prevent graph from popping up and block=False replaced it along with plt.pause
# the pause functions for how many seconds to wait until it closes graph
plt.show(block=False)
plt.pause(15)
