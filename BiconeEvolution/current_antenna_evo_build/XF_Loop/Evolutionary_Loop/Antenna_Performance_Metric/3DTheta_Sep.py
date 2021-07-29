#Plot theta in 3D

import numpy as np		# for data manipulation, storage
import matplotlib.pyplot as plt	# For plotting
from mpl_toolkits.mplot3d import Axes3D   #for 3D plotting
import os			# exclusively for rstrip()
import argparse			# for getting the user's arguments from terminal


parser = argparse.ArgumentParser()
parser.add_argument("source", help="Name of source folder from home directory", type=str)
parser.add_argument("destination", help="Name of destination folder from home directory", type=str)
parser.add_argument("numGens", help="Number of generations the code is running for", type=int)
parser.add_argument("NPOP", help="Number of individuals in a population", type=int)
parser.add_argument("GeoScalingFactor", help="The number by which we are scaling the antenna dimensions", type=int)
parser.add_argument("NSECTIONS", help="Number of chromosomes", type=int)
g = parser.parse_args()


# First, grab each line of the runData.csv as one element in a 1D list.
runDataRaw =[]
with open(g.source + "/runData.csv", "r") as runDataFile:
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
#print("RawOnlyNumb ")
#print(runDataRawOnlyNumb)
# Now convert it to a numpy array and roll it up
runData = []
runData = np.array(runDataRawOnlyNumb)
#print("runData ")
#print(runData)
runData = np.array(runDataRawOnlyNumb).astype(np.float)
#print("runData ")
#print(runData)
runData = runData.reshape((g.numGens, g.NPOP, 4*g.NSECTIONS))
#The 5 above is (NVARS+1), where the +1 accounts for fitness scores appended by gensData
#runData = np.array(runData, np.float).reshape(g.numGens, g.NPOP, 4)
# Finally, the data is in an almost useable shape: (generation, individual, characteristic)

# PLOT DATA

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


#Create Array for every fitness score
fitnessArray = runData[:,:, 3].flatten()

# Plot!
#Create figure and subplots
fig = plt.figure(figsize=(20, 6))
ax = plt.axes(projection='3d')

map = plt.get_cmap('winter')

# Loop through each individual and plot each array
#color={1:'red',2:'olive',3:'mediumturquoise',4:'blue',5:'gold',6:'darkred',7:'green',8:'lime',9:'orange',10:'indigo',11:'dimgrey',12:'rosybrown',13:'lightcoral',14:'firebrick',15:'maroon',16:'sienna',17:'sandybrown',18:'peachpuff',19:'peru',20:'tan'}
for ind in range(g.NPOP):
	LabelName = "Individual {}".format(ind+1)
	ax.scatter3D(theta1Array[ind], theta2Array[ind], fitnessArray[ind], cmap = map)


# Labels:

#Theta subplot
#axT.set(xlabel='Generation', ylabel = 'Theta [Degrees]')
ax.set_xlabel("Theta 1 [Degrees]", size = 18)
ax.set_ylabel("Theta 2 [Degrees]", size = 18)
ax.set_title("Theta 1 & 2 over Fitness Score".format(int(g.numGens-1)), size = 20)

plt.savefig("3DTheta")
#plt.show(block=False)
#plt.pause(5)

