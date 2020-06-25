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
parser.add_argument("NSECTIONS", help="Number of chromosomes", type=int)
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
	if i%((g.NPOP*g.NSECTIONS)+2) != 0 and i%((g.NPOP*g.NSECTIONS)+2) != 1:
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
runData = runData.reshape((g.numGens, g.NPOP, 4*g.NSECTIONS))
#runData = np.array(runData, np.float).reshape(g.numGens, g.NPOP, 4)
# Finally, the data is in an almost useable shape: (generation, individual, characteristic)

# PLOT DATA

# Create an array of length 1
allLength1 = runData[:,:, 1].flatten()
# The loop below converts the messy lengths array into a cleaner array of arrays:
# lengths = [I1_G0, I2_G0, I3_G0, I1_G1, I2_G1....]
# to
# lengthArray = [ [I1_G0, I1_G1, I1_G2...], [I2_G0, I2_G1, I2_G2...], ...]
length1Array = []
templength1 = [] 
for ind in range(g.NPOP):
    for l in range(0,len(allLength1),g.NPOP):
        templength1.append(g.GeoScalingFactor*allLength1[l+ind])
    length1Array.append(templength1)
    templength1 = []

#Create an array of lenght 2
allLength2 = runData[:,:, 5].flatten()

length2Array = []
templength2 = [] 
for ind in range(g.NPOP):
    for l in range(0,len(allLength2),g.NPOP):
        templength2.append(g.GeoScalingFactor*allLength2[l+ind])
    length2Array.append(templength2)
    templength2 = []

# Create an array of every theta1
allTheta1 = runData[:,:, 2].flatten()

theta1Array = []
temptheta1 = []
for ind in range(g.NPOP):
    for l in range(0,len(allTheta1),g.NPOP):
            temptheta1.append(allTheta1[l+ind]*180/np.pi)
    theta1Array.append(temptheta1)
    temptheta1 = []

# Create an array of every theta2
allTheta2 = runData[:,:, 6].flatten()

theta2Array = []
temptheta2 = []
for ind in range(g.NPOP):
    for l in range(0,len(allTheta2),g.NPOP):
            temptheta2.append(allTheta2[l+ind]*180/np.pi)
    theta2Array.append(temptheta2)
    temptheta2 = []

# Create an array of every radius
allRadii = runData[:,:, 0].flatten()

radiiArray = []
tempradii = []
bigRadii1 = [] # for holding the outer radius of each individual
tempBigRadii1 = []
bigRadii2 = []
tempBigRadii2 = [] 
for ind in range(g.NPOP):
    for l in range(0,len(allRadii),g.NPOP):
            tempradii.append(g.GeoScalingFactor*allRadii[l+ind])
            tempBigRadii1.append(g.GeoScalingFactor*(allRadii[l+ind] + allLength1[l+ind]*np.tan(allTheta1[l+ind])))
            tempBigRadii2.append(g.GeoScalingFactor*(allRadii[l+ind] + allLength2[l+ind]*np.tan(allTheta2[l+ind]))) #I need to think about if this is the smartest way to populate this list -- Machtay 2/11/20
    radiiArray.append(tempradii)
    tempradii = []
    bigRadii1.append(tempBigRadii1)
    tempBigRadii1 = []
    bigRadii2.append(tempBigRadii2)
    tempBigRadii2 = []

# Plot!
#Create figure and subplots
fig = plt.figure(figsize=(20, 6))
axL = fig.add_subplot(1,3,1)
axR = fig.add_subplot(1,3,2)
axT = fig.add_subplot(1,3,3)
#axO = fig.add_subplot(1,4,4)

# Loop through each individual and plot each array
color={1:'red',2:'olive',3:'mediumturquoise',4:'blue',5:'gold',6:'darkred',7:'green',8:'lime',9:'orange',10:'indigo'}
for ind in range(g.NPOP):
	LabelName = "Individual {}".format(ind+1)
	axL.plot(length1Array[ind], color=color.get(ind+1, 'black'), marker = 'o', label = LabelName, linestyle = '')
	axL.plot(length2Array[ind], color=color.get(ind+1, 'black'), marker = 'x', label = LabelName, linestyle = '')
	axR.plot(radiiArray[ind], color=color.get(ind+1, 'black'), marker = 'o', label = LabelName, linestyle = '')
	axT.plot(theta1Array[ind], color=color.get(ind+1, 'black'), marker = 'o', label = LabelName, linestyle = '')
	axT.plot(theta2Array[ind], color=color.get(ind+1, 'black'), marker = 'x', label = LabelName, linestyle = '')
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
#axL.legend()
#axR.legend()
#axT.legend()
#axO.legend()

plt.savefig(g.destination + "/" + PlotName)
plt.show(block=False)
plt.pause(5)

fig = plt.figure(figsize = (10, 8))
for i in range(g.NPOP):
    LabelName = "Individual {}".format(ind+1)
    plt.plot(bigRadii1[i], color=color.get(i+1, 'black'), marker = 'o', label = LabelName, linestyle = '')
    plt.plot(bigRadii2[i], color=color.get(i+1, 'black'), marker = 'x', label = LabelName, linestyle = '')
plt.xlabel('Generation')
plt.ylabel('Outer Radius [cm]')
plt.title('Outer Radius vs. Generation')
plt.savefig(g.destination + "/" + "Outer_Radii")
plt.show(block=False)
plt.pause(5)
