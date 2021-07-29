# Written by: 	Suren Gourapura
# Written on: 	Dec 21, 2018
# Reads from: 	generationDNA.csv, fitnessScores.csv
# Writes to: 	maxFitnessScores.csv, runData.csv (formerly gensData.csv)
# Purpose:	Take data from fitness algorithm and record relevant data: the maximum fitness score and the total information for each antenna. Dynamically adjusts for any NPOP.

import numpy as np
import argparse
import math

#---------GLOBAL VARIABLES----------GLOBAL VARIABLES----------GLOBAL VARIABLES----------GLOBAL VARIABLES

# We need to grab the generation number this code is being run on from the bash script
parser = argparse.ArgumentParser()

parser.add_argument("GenNumber", help="Generation number the code is running on (for formatting purposes)", type=int)
parser.add_argument("NSECTIONS", help="Number of chromsomes", type=int)
parser.add_argument("NPOP", help="Number of individuals", type=int)
parser.add_argument("location", help="Location of runData.csv file", type=str)
g = parser.parse_args()

#----------STARTS HERE----------STARTS HERE----------STARTS HERE----------STARTS HERE 


# READ DATA
# Read maxFitnessScores.csv, generationDNA.csv, and fitnessScores.csv

# We use loadtxt when we need to skip over the useless text
genDNA = np.loadtxt(g.location + "/generationDNA.csv", delimiter=',', skiprows=9)
fScores = np.loadtxt(g.location + "/fitnessScores.csv", delimiter=',', skiprows=2)

# We need to copy fScores into a new array that holds each score twice in a row, so as to account for the genDNA which has two lines(1 for each chromsome) per individual
fScores2 = []
for i in range(0, g.NPOP*g.NSECTIONS):
	j = math.floor(i/g.NSECTIONS)
	fScores2.append(fScores[j])

fScores3 = np.array(fScores2)
# Create/Add to runData.csv
# This file contains every antenna's DNA and fitness score for each generation. 
# Format for each individual is radius, length, angle, fitness score. See below example:
'''
Generation :0
1.122650,19.905200,0.504576,32.500000
0.478846,7.547800,0.059359,37.629900
1.194750,25.860000,0.110638,27.450000
1.368760,13.599100,0.269844,15.370000
1.008510,3.146630,0.711409,45.320000
0.824155,12.455700,0.297416,22.630000
1.441790,13.586500,0.459830,41.900000
0.512955,25.759700,0.366288,39.190000
0.486734,28.389000,1.063220,26.620000
0.957653,24.559700,0.319919,23.430000

Generation :1
1.122650,19.905200,0.504576,32.500000
0.478846,7.547800,0.059359,37.629900
1.194750,25.860000,0.110638,27.450000
'''

# First, stick genDNA with fScores to have the correct matrix format described above
genDNAfScore = np.hstack(( genDNA, fScores3.reshape((fScores3.shape[0], 1)) ))
# Open the file and append the relevant information
with open(g.location + "/runData.csv", "a") as runData:
	runData.write('\n'+ "Generation :"+ str(g.GenNumber)+ '\n')
	np.savetxt(runData, genDNAfScore, delimiter=",", fmt='%f')


# Create/Add to maxFitnessScores.csv
# This file contains just the maximum fitness score for each generation.

# First, we need to find the maximum fitness score
maxFScore = fScores.max()
# Then, append this onto the maxFitnessScores list
with open(g.location + "/maxFitnessScores.csv", "a") as maxFScores:
	maxFScores.write("Generation "+str(g.GenNumber)+"'s Max Fitness Score: "+str(maxFScore)+ '\n')



