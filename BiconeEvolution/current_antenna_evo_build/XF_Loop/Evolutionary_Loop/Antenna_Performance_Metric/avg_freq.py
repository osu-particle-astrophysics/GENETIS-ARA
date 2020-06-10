## Filename: avg_freq.py
## Author: Alex Machtay (machtay.1@osu.edu)
## Date: 6/1/20
#
#
#
#
#
### Purpose:
## The goal of this program is to read gains from XF of antennas for each frequency so that
## the average gain can be compared between antennas at the same frequency.
#
#
#
#
#
## To run this script, pass the following arguments:
## 1. source (where the file is located)
## 2. destination (where to output any files from this program) **(currently not used)**
## 3. numFreqs -- The number of frequencies being checked **(currently not used)**
## 4. NPOP--the number of individuals you want to plot
#
## Run as follows:
## python3 avg_freq.py (source) (destination) (numFreqs) (NPOP)
## For example:
## python3 avg_freq.py . . 10 1



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
parser.add_argument('freqs', type=int)
parser.add_argument('NPOP', type=int)
g = parser.parse_args()

# begin code

# we're ultimately going to need lists of lists, so start by defining them
all_avg_gains = [] 		# holds each avg gain list for each antenna
all_avg_dB = []				# holds each avg dB list for each antenna
all_avg_phase = []

for k in range(1, g.NPOP+1):
	freq_list = [] 		# we're going to get a list of the frequencies
	avg_gain = [] 		# for each frequency we want to get the average gain 
	avg_dB = [] 			# it might be useful to have the frequencies in dB
	temp_gain = [] 		# temporarily holds the gain before taking the average
	temp_dB = [] 			# temporarily holds the dB before taking the average
	avg_phase = []		# for each frequency we also want the phase
	temp_phase = [] 	# temporarily holds the phase during the loop before taking the average

	with open(g.source + "/a_" + str(k) + ".txt") as f1:
		txt_read = csv.reader(f1, delimiter = " ") #the delimiter is a space
		for i, row in enumerate(txt_read):
			if row[0] == "freq": # this is how we know we have a new frequency
				# once we have the temporary gains of a frequency, we want to put their mean in the list
				if len(freq_list) > 0:
					avg_gain.append(mean(temp_gain)) 		# appending the mean of the gains to avg_gain
					avg_dB.append(mean(temp_dB)) 				# appending the mean of the dBs to avg_dB
					avg_phase.append(mean(temp_phase)) 	# appending the mean of the phase 

				freq_list.append(float(row[2])) # add in the new frequency
				

				if len(freq_list) == 60:
					temp_gain = [] 			# empty the temporary gain
					temp_dB = [] 				# empty the temporary dB
					temp_phase = [] 		# empty the temporary phase

			if row[1] != "Theta" and row[0] != "freq" and row[0] != "SWR":
				#print(row[0], ",", row[4], ",", row[12]) 
				# the .txt files are written strangely
				# the delimiter uses tabs, but that's not recognized by csv reader
				# so csv reader sees those as in-line values, meaning we have to skip them when using row
				# additionally, the delimiter switches midway through the line!
				# so the arguments below look weird, but they are correct

				temp_gain.append(float(row[12])) 			# append the gain
				temp_dB.append(float(row[4])) 				# append the dB
				temp_phase.append(float(row[-1])) 		# append the phase
	f1.close()

	# we have to make sure we get the last data point
	# we don't get it from the above loop because it's nested in where the frequency is declared
	# we just do the same thing we did above

	avg_gain.append(mean(temp_gain)) 			# appending the mean of the gains to avg_gain
	avg_dB.append(mean(temp_dB)) 					# appending the mean of the dBs to avg_dB
	avg_phase.append(mean(temp_phase)) 		# appending the mean phases to avg_phase
	# now we'll add to our master lists

	all_avg_gains.append(avg_gain)
	all_avg_dB.append(avg_dB)
	all_avg_phase.append(avg_phase)

print(len(all_avg_gains))
print(len(all_avg_dB))
print(len(all_avg_phase))


# plot the gain vs frequency

fig = plt.figure(figsize = (10, 6))

for ind in range(0, g.NPOP):
	labelName = "Individual {}".format(str(ind+1))
	plt.plot(freq_list, all_avg_gains[ind], marker = 'o', linestyle = '', label = labelName)

plt.xlabel("Frequency (MHz)", size = 30)
plt.ylabel("Average Gain", size = 30)
plt.title("Average Gain at Each Frequency")
plt.legend()
plt.savefig(g.destination + "/" + "gain_vs_freq.png")


# plot the phase vs gain

fig2 = plt.figure(figsize = (10, 6))

for ind in range(0, g.NPOP):
	labelName = "Individual {}".format(str(ind+1))
	plt.plot(freq_list, all_avg_phase[ind], marker = 'o', linestyle = '', label = labelName)

plt.xlabel("Frequency (MHz)", size = 30)
plt.ylabel("Average Phase (degrees)", size = 30)
plt.title("Average Phase at Each Frequency")
plt.legend()
plt.savefig(g.destination + "/" + "phase_vs_freq.png")


