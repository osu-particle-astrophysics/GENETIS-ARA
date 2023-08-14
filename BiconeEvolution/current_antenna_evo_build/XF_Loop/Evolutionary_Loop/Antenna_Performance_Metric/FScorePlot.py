import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import argparse
import csv
import matplotlib.cm as cm
 
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
#Plot3DName = "/FScorePlot3D.png"

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
        fitnesses = "Generation_Data/" + "Generation_{}".format(gen) + "/{}_fitnessScores.csv".format(gen)
        #for each generation, we need to get all the fitnesses
        with open(g.source + "/" + fitnesses, "r") as fr: #fr for fitnesses read
            f_read = csv.reader(fr, delimiter=',') #reading fr as a csv
            for i, row in enumerate(f_read): #loop over the rows 
                if i == lineNum: #skipping the header
                    fitness = float(row[0]) #lineNum contains the fitness score
                    #print(fitness)
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
fpActual = open(g.source + "/Generation_Data/Generation_0/" + filenameActual)

for line in fpActual:
    if "test Veff(ice) : " in line:
        Veff_ARA = float(line.split()[5]) #changed from 3 to 5 for switching to km^3 from m^3
        #print(Veff_ARA)
    elif "And Veff(water eq.) error plus :" in line:
        Err_plus_ARA = float(line.split()[6])
        Err_minus_ARA = float(line.split()[11])
#    line = fpActual.readline()
    #print(line)
fpActual.close()

## Adding line of average fitness score
MeanFitness = []
MeanError = []
FlippedFitness = np.transpose(FitnessesArray)
#print(FlippedFitness)
for ind in range(g.numGens+1):	
	mean = sum(FlippedFitness[ind])/g.NPOP
	MeanFitness.append(mean)	
	MeanError.append(np.sqrt(np.sum((FlippedFitness[gen] - mean)**2)/((g.NPOP-1)*(g.NPOP))))

## Adding line of median fitness score
MedianFitness = []
for gen in range(g.numGens+1):
        median = np.median(FlippedFitness[gen])
        MedianFitness.append(median)


#Veff_ARA_Ref = Veff_ARA * np.ones(len(genAxis))

plt.figure(figsize=(10, 8))
#plt.plot(genAxis, Veff_ARA_Ref, label = "ARA Reference", linestyle= '--', color = 'k')
plt.axhline(y=Veff_ARA, linestyle = '--', color = 'k')
#plt.axes([-1, g.numGens+1, -1, 6])
#plt.xlabel('Generation', size = 22)
#plt.ylabel('Fitness Score', size = 22)
#plt.title("Fitness Score over Generations (0 - {})".format(int(g.numGens)), size = 25)
colors = cm.rainbow(np.linspace(0, 1, g.NPOP))
plt.axis([-1, g.numGens+1, -0.5, np.max(FitnessesArray) + np.max(Err_plus_ARA) + 0.5])

for ind in range(g.NPOP):
		LabelName = "Individual {}".format(ind+1)
		plt.plot(genAxis, FitnessesArray[ind], label = LabelName, marker = 'o', color = colors[ind], linestyle='', alpha = 0.4, markersize = 11)
		#plt.plot(genAxis, FitnessesArray[ind], label = LabelName, marker = 'o', color = 'k', linestyle='', alpha = 0.8, markersize = 9)
		plt.plot(genAxis, MeanFitness, label = LabelName, linestyle='-', alpha = 1, markersize = 15)
plt.xlabel('Generation', size = 26)
plt.ylabel('Fitness Score (km$^3$str)', size = 26)
plt.title("Fitness Score over Generations (0 - {})".format(int(g.numGens)), size = 30)

#plt.legend()
plt.savefig(g.destination + Plot2DName)

## Let's make a violin plot
fig = plt.figure(figsize = (24, 12))
ax = fig.add_subplot(111)

#ax.axis([-1, g.numGens+1, np.min(FitnessesArray) - 1*np.max(Err_plus_ARA), np.max(FitnessesArray) + np.max(Err_plus_ARA)])

ax.axis([-1, g.numGens+1, 3, 5.5])

print(len(FlippedFitness))
print(len(genAxis))

violin_parts = ax.violinplot(list(FlippedFitness), genAxis, showmeans = False, showextrema = False, showmedians = False)
for pc in violin_parts['bodies']:
	pc.set_facecolor('#7570b3')
	pc.set_alpha(0.7)
ax.plot(genAxis, MeanFitness, linestyle = '-', color = '#d95f02', label = "Mean", linewidth = 2)
ax.plot(genAxis, MedianFitness, linestyle = '-.', color = '#1b9e77', label = "Median", linewidth = 3, markersize = '10', alpha = 1)
ax.fill_between(genAxis, np.array(MeanFitness) - np.array(MeanError), np.array(MeanFitness) + np.array(MeanError), color='#d95f02', alpha=0.2)
ax.axhline(y=Veff_ARA, linestyle = '--', color = 'k', label = "ARA Bicone", linewidth = 2)
for i in range(0, g.numGens+1):
	ax.vlines(i, ymin = np.min(FlippedFitness[i]), ymax = np.max(FlippedFitness[i]))

#quartile1, medians, quartile3 = np.percentile(FitnessesArray, [25, 50, 75], axis=1)
#inds = np.arange(1, len(medians)+1)
#ax.vlines(inds, whiskersmin, whiskersmax, color = 'k', linestyle = '--', lw = 1)

ax.legend(loc = 'lower right', prop={'size': 26}, framealpha=0.5)
ax.set_xticks(np.arange(0, g.numGens + 1, 5.0))
plt.xticks(fontsize = 18)
plt.yticks(fontsize = 18)
plt.xlabel('Generation', size = 32)
plt.ylabel('Fitness Score (V$_e$$_f$$_f$ (km$^3$sr))', size = 32)#'Fitness Score (km$^3$str)', size = 26)
plt.title("Fitness Score over Generations (0 - {})".format(int(g.numGens)), size = 36)
	
plt.savefig(g.destination + "/Violin_Plot.png", bbox_inches='tight')

#for x in range (len(fScoresInd[1])):
    #print fScoresInd[1][x]

##plt.show()
# was commented out to prevent graph from popping up and block=False replaced it along with plt.pause
# the pause functions for how many seconds to wait until it closes graph
#plt.show(block=False)
#plt.pause(15)

#plt.figure(figsize=(10, 8))
#indAxis = np.linspace(1,g.NPOP,g.NPOP)
#genAxis, indAxis = np.meshgrid(genAxis, indAxis)
#ax = plt.axes(projection='3d')
#ax.plot_surface(genAxis, indAxis, fScoresInd, rstride=1, cstride=1, cmap='viridis', edgecolor='none')
#ax.set_title('3D Fitness Score over Generations');
#ax.set_xlabel('Generation')
#ax.set_ylabel('Individual')
#ax.set_zlabel('Fitness Score')
#plt.savefig(g.destination + Plot3DName)
##plt.show()
# was commented out to prevent graph from popping up and block=False replaced it along with plt.pause
# the pause functions for how many seconds to wait until it closes graph
##plt.show(block=False)
#plt.pause(15)
