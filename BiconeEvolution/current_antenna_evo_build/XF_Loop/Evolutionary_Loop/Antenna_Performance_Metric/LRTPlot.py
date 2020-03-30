#This script is largely a copy of the LRPlot.py script, which plots the length and radius of the
#individuals--here, we want to do the same thing but also add in a plot for the Theta values

# Written by: 	Suren Gourapura
# Written on: 	February 25, 2019
# Purpose: 	Plot the length and radius from each generation and the ones before. Give a 2D plot to the users and save it to g.destinations

import numpy as np		# for data manipulation, storage
import matplotlib.pyplot as plt	# For plotting
import os			# exclusively for rstrip()
import argparse			# for getting the user's arguments from terminal
# May be needed: from mpl_toolkits.mplot3d import Axes3D 

#---------GLOBAL VARIABLES----------GLOBAL VARIABLES----------GLOBAL VARIABLES----------GLOBAL VARIABLES


# We need to grab the three arguments from the bash script or user. These arguments in order are [the name of the source folder of the fitness scores], [the name of the destination folder for the plots], and [the number of generations] #why is is number of generations and not gen number??
parser = argparse.ArgumentParser()
parser.add_argument("source", help="Name of source folder from home directory", type=str)
parser.add_argument("destination", help="Name of destination folder from home directory", type=str)
parser.add_argument("numGens", help="Number of generations the code is running for", type=int)
parser.add_argument("NPOP", help="Number of individuals in a population", type=int)
parser.add_argument("GeoScalingFactor", help="The number by which we are scaling the antenna dimensions", type=int)
g = parser.parse_args()

# The name of the plot that will be put into the destination folder, g.destination
PlotName = "LRTPlot2D"


#----------DEFINITIONS HERE----------DEFINITIONS HERE----------DEFINITIONS HERE----------DEFINITIONS HERE
#----------STARTS HERE----------STARTS HERE----------STARTS HERE----------STARTS HERE 


# READ DATA (runData.csv)

# runData.csv contains every antenna's DNA and fitness score for all generations. Format for each individual is radius, length, angle, fitness score (I call these characteristics).

# First, grab each line of the runData.csv as one element in a 1D list.
runDataRaw =[]
with open(g.source+"/runData.csv", "r") as runDataFile:
	runDataRaw=runDataFile.readlines()

# This list has each element terminating with '\n', so we use rstrip to remove '\n' from each string
for i in range(len(runDataRaw)):
	runDataRaw[i] = runDataRaw[i].rstrip()
# Now, we want to store this data in a 2D numpy array. As we'll see, this is a fairly complex process! First, make a new 2D list that contains only the numbers.
runDataRawOnlyNumb =[]
for i in range(len(runDataRaw)):
	# We want to skip the empty line and the 'Generation :' line
	if i%(g.NPOP+2) != 0 and i%(g.NPOP+2) != 1:
		# The split function takes '1.122650,19.905200,0.504576,32.500000' -> ['1.122650', '19.905200', '0.504576', '32.500000'] , which makes the new list 2D
		runDataRawOnlyNumb.append(runDataRaw[i].split(','))#.astype(float) 
print("RawOnlyNumb ")
print(runDataRawOnlyNumb)
# Now convert it to a numpy array and roll it up
runData = []
runData = np.array(runDataRawOnlyNumb)
print("runData ")
print(runData)
runData = np.array(runDataRawOnlyNumb).astype(np.float)
print("runData ")
print(runData)
runData = runData.reshape((g.numGens, g.NPOP, 4))
#runData = np.array(runData, np.float).reshape(g.numGens, g.NPOP, 4)
# Finally, the data is in an almost useable shape: (generation, individual, characteristic)


# PLOT DATA

# Create an array of every length
allLengths = runData[:,:, 1].flatten()

# The loop below converts the messy lengths array into a cleaner array of arrays:
# lengths = [I1_G0, I2_G0, I3_G0, I1_G1, I2_G1....]
# to
# lengthArray = [ [I1_G0, I1_G1, I1_G2...], [I2_G0, I2_G1, I2_G2...], ...]
lengthsArray = []
templength = [] 
for ind in range(g.NPOP):
    for l in range(0,len(allLengths),g.NPOP):
        templength.append(g.GeoScalingFactor*allLengths[l+ind])
    lengthsArray.append(templength)
    templength = []


# Create an array of every theta
allThetas = runData[:, :, 2].flatten()

thetasArray = []
tempthetas = []
for ind in range(g.NPOP):
    for l in range(0,len(allThetas),g.NPOP):
            tempthetas.append(allThetas[l+ind]*180/np.pi)
    thetasArray.append(tempthetas)
    tempthetas = []


# Create an array of every radius
allRadii = runData[:,:, 0].flatten()

radiiArray = []
tempradii = []
bigRadii = [] # for holding the outer radius of each individual
tempBigRadii = [] 
for ind in range(g.NPOP):
    for l in range(0,len(allRadii),g.NPOP):
            tempradii.append(g.GeoScalingFactor*allRadii[l+ind])
            tempBigRadii.append(g.GeoScalingFactor*(allRadii[l+ind] + allLengths[l+ind]*np.tan(allThetas[l+ind]))) #I need to think about if this is the smartest way to populate this list -- Machtay 2/11/20
    radiiArray.append(tempradii)
    tempradii = []
    bigRadii.append(tempBigRadii)
    tempBigRadii = []

# Plot!
#Create figure and subplots
fig = plt.figure(figsize=(20, 6))
axL = fig.add_subplot(1,3,1)
axR = fig.add_subplot(1,3,2)
axT = fig.add_subplot(1,3,3)
#axO = fig.add_subplot(1,4,4)

# Loop through each individual and plot each array
for ind in range(g.NPOP):
	LabelName = "Individual {}".format(ind+1)
	axL.plot(lengthsArray[ind], marker = 'o', label = LabelName, linestyle = '')
	axR.plot(radiiArray[ind], marker = 'o', label = LabelName, linestyle = '')
	axT.plot(thetasArray[ind], marker = 'o', label = LabelName, linestyle = '')
	#axO.plot(bigRadii[ind], marker = 'o', label = LabelName, linestyle = '')

# Labels:
#Length subplot
#axL.set(xlabel='Generation', ylabel = 'Length [cm]')
axL.set_xlabel("Generation", size = 18)
axL.set_ylabel("Length [cm]", size = 18)
axL.set_title("Length over Generations (0 - {})".format(int(g.numGens-1)), size = 20)

#Radius subplot
#axR.set(xlabel='Generation', ylabel = 'Radius [cm]')
axR.set_xlabel("Generation", size = 18)
axR.set_ylabel("Radius [cm]", size = 18)
axR.set_title("Radius over Generations (0 - {})".format(int(g.numGens-1)), size = 20)

#Theta subplot
#axT.set(xlabel='Generation', ylabel = 'Theta [Degrees]')
axT.set_xlabel("Generation", size = 18)
axT.set_ylabel("Theta [Degrees]", size = 18)
axT.set_title("Theta over Generations (0 - {})".format(int(g.numGens-1)), size = 20)

"""
#Outer Radius subplot
#axL.set(xlabel='Generation', ylabel = 'Length [cm]')
axO.set_xlabel("Generation", size = 18)
axO.set_ylabel("Length [cm]", size = 18)
axO.set_title("Outer Radius over Generations (0 - {})".format(int(g.numGens-1)), size = 20)
#I also want to put in the ARA bicone reference at some point
#axO.axhline(y=Veff_ARA, linestyle = '--', color = 'k')
"""

#axL.set_title("Length over Generations (0 - {})".format(int(g.numGens-1)))
#axR.set_title("Radius over Generations (0 - {})".format(int(g.numGens-1)))
#axT.set_title("Theta over Generations (0 - {})".format(int(g.numGens-1)))

#Set the legends
axL.legend()
axR.legend()
axT.legend()
#axO.legend()

plt.savefig(g.destination + "/" + PlotName)
plt.show(block=False)
plt.pause(5)

fig = plt.figure(figsize = (10, 8))
for i in range(g.NPOP):
    LabelName = "Individual {}".format(ind+1)
    plt.plot(bigRadii[i], marker = 'o', label = LabelName, linestyle = '')
plt.xlabel('Generation')
plt.ylabel('Outer Radius [cm]')
plt.title('Outer Radius vs. Generation')
plt.savefig(g.destination + "/" + "Outer_Radii")
plt.show(block=False)
plt.pause(5)
