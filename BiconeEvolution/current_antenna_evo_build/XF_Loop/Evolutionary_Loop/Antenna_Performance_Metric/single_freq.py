## Filename: single_freq.py
## Author: Alex Machtay (machtay.1@osu.edu)
## Date: 6/30/20
#
#
#
#
#
### Purpose:
## The goal of this program is to read gains from XF of antennas for each frequency so that
## the gain at a specific polar angle can be plotted.
#
#
#
#
#
## To run this script, pass the following arguments:
## 1. source (where the file is located)
## 2. destination (where to output any files from this program) **(currently not used)**
## 3. maxFreq -- The number of frequencies being checked 
## 4. NPOP--the number of individuals you want to plot
## 5. theta_angle--the polar angle you want to check the gain at (increment by 5)
#
## Run as follows:
## python3 avg_freq.py (source) (destination) (maxFreq) (NPOP) (angle)
## For example (from inside the run name directory):
## python3 ../../Antenna_Performance_Metric/single_freq.py . . 60 10 90



# imports

import numpy as np
import csv
import argparse
import matplotlib.pyplot as plt
from statistics import mean

# arguments

parser = argparse.ArgumentParser()
parser.add_argument('source', type=str)
parser.add_argument('destination', type=str)
parser.add_argument('maxFreq', type=int)
parser.add_argument('NPOP', type=int)
parser.add_argument('theta_angle', type=str)
g = parser.parse_args()

# begin code

all_gains = []			# this will hold the gains for every frequency for every individual
gains = []					# temporarily holds the gain for every frequency for an individual
decibel = []				# temporarily holds dBi for every frequency for an individual
all_decibel = []		# holds all of the gains in dBi for every frequency for every individual
for k in range(1, g.NPOP+1):
	freq_list = [] 			# this will hold the frequencies
	freq_gain = []			# this will hold the gain ata given frequency
	freq_dB = []				# this will hold the dBi at a given frequency
	with open(g.source + "/a_" + str(k) + ".txt") as f1:
		txt_read = csv.reader(f1, delimiter = " ") #the delimiter is a space
		for i, row in enumerate(txt_read):
			if row[0] == "freq": # this is how we know we have a new frequency
				freq_list.append(float(row[2])) # add in the new frequency
				if len(freq_list) > 1:
					gains.append(freq_gain)
					decibel.append(freq_dB)
				freq_gain = []
				freq_dB = []

			# if the theta angle is 90 degrees, we want to add it in
			if row[0] == g.theta_angle: 
				freq_gain.append(float(row[12]))	# append the gain
				freq_dB.append(float(row[4]))		# append the dBi
			if len(freq_list) > g.maxFreq:
				gains.append(freq_gain)
				decibel.append(freq_dB)
				break

		f1.close()

	all_gains.append(gains)
	all_decibel.append(decibel)

	gains = []
	decibel = []

# uncomment the below print statements to check things look good
# freq list should be g.maxFreq+1
# all_gains should be NPOP
# all_gains[i] should be 68
# all gains[i][j] should be 72
print(len(freq_list))
print(len(all_gains))
print(len(all_gains[0]))
print(len(all_gains[0][0]))
print(len(all_decibel))
print(len(all_decibel[0]))
print(len(all_decibel[0][0]))
print(all_gains[0][0])
print(all_decibel[0][0])

'''
many_freqs = []

i = 0
while i < 68:
	many_freqs.append(freq_list)
	i += 1
'''


# plot the gain vs frequency

fig = plt.figure(figsize = (10, 6))


for ind in range(0, g.NPOP):
	#labelName = "Individual {}".format(str(ind+1))
	color_cycle = ['C0', 'C1', 'C2', 'C3', 'C4', 'C5', 'C6', 'C7', 'C8', 'C9']
	i = 0
	while i < g.maxFreq:
		j = 0
		while j < 72:
			plt.plot(freq_list[i], all_gains[ind][i][j], marker = 'o', linestyle = '', color = color_cycle[ind - 1])#label = labelName)
			j += 1
		i += 1


plt.xlabel("Frequency (MHz)", size = 30)
plt.ylabel("Gain", size = 30)
plt.title("Gain at Each Frequency at Theta = {} degrees".format(g.theta_angle))
#plt.legend()
plt.savefig(g.destination + "/" + "single_gain_vs_freq.png")



# plot the dBi vs frequency

fig = plt.figure(figsize = (10, 6))


for ind in range(0, g.NPOP):
	#labelName = "Individual {}".format(str(ind+1))
	color_cycle = ['C0', 'C1', 'C2', 'C3', 'C4', 'C5', 'C6', 'C7', 'C8', 'C9']
	i = 0
	while i < g.maxFreq:
		j = 0
		while j < 72:
			plt.plot(freq_list[i], all_decibel[ind][i][j], marker = 'o', linestyle = '', color = color_cycle[ind - 1])#label = labelName)
			j += 1
		i += 1


plt.xlabel("Frequency (MHz)", size = 30)
plt.ylabel("Gain (dBi)", size = 30)
plt.title("Gain at Each Frequency at Theta = {} degrees".format(g.theta_angle))
#plt.legend()
plt.savefig(g.destination + "/" + "single_dBi_vs_freq.png")

