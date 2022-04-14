## File name: polar_plotter.py
## Author: Alex Machtay (machtay.1@osu.edu)
## Date: 07/19/21
## Editors: Audrey Zinn (zinn.60@osu.edu), Bailey Stephens (stephens.761@osu.edu)
## Edit Date: 11/04/21
## Purpose:
##	This script is designed to read in the results from XF and plot the gain patterns.
##	It will plot each of the antennas given (in a range) at a specific (given) frequency.
##	These will show the gain at zenih angles, since the patterns are azimuthally symmetric
#
#
#
## Instructions:
## 		To run, give the following arguments
##			source directory, destination directory, frequency number, # of individuals per generation, generation #
#
## 		Example:
##			python polar_plotter.py Length_Tweak Length_Tweak 60 10 0
## 				This will plot the antennas for the length tweak from number 1-10
##				(which is all of them) at the highest frequency number (1067 MHz) and
##				will place the image inside the Length_Tweak directory.
#
## Imports
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import argparse
import csv
import matplotlib.cm as cm
#
#
#
## Arguments
parser = argparse.ArgumentParser()
parser.add_argument("source", help="Name of source folder from home directory", type=str)
parser.add_argument("destination", help="Name of destination folder from home directory", type=str)
parser.add_argument("freq_num", help="Frequency number (1-60) to plot", type=int)
parser.add_argument("NPOP", help="Number of individuals in a generation (ex: 10)", type=int)
parser.add_argument("gen", help="Generation number (ex: 0)", type=int)
g=parser.parse_args()
#
#
#
## Loop over files
# Declare list for each file's gain list
gains = []
for individual in range(1, g.NPOP+1):
	## Open the file to read
	## NOTE: Looks for folder in source that contains a folder with name "gen_#"
	with open(g.source + "/" + str(g.gen) + "_uan_files/" + str(g.gen) + "_" + str(individual) + "_" + str(g.freq_num) + ".uan", "r") as f:
		## Read in the file
		uan_read = csv.reader(f, delimiter = ' ')
		## Declare list to hold azimuth gains
		azimuth_gain = []
		for m, row in enumerate(uan_read):
			## skip the header
			if m >= 17:
				## Let's just add the azimuthal angles
				azimuth_gain.append(float(row[2]))
	f.close()
	## Declare temporary zenith gains
	zenith_gains = []
	## Loop over the zenith angles
	for j in range(1, 36):
		## Declare temporary azimuth means
		azimuth_mean = []
		## Loop over the azimuth angles
		k = j*73
		while k < ((j+1)*73):
			azimuth_mean.append(azimuth_gain[k])
			## Remember to increment!
			k += 1
		zenith_gains.append(np.mean(azimuth_mean))
		## Remember to increment!
	## Append the list of mean azimuth gains at each zenith to the gains list
	gains.append(zenith_gains)

print(len(azimuth_gain))
print(len(gains))
print(len(gains[0]))
#
#
#
## Plotting
# Make a list for the zenith angles
zenith_angles = []
for i in range(1, 36):
	zenith_angles.append(i*5*np.pi/180)
#
#
#
## Declare a figure
colors = cm.rainbow(np.linspace(0, 1, g.NPOP))
fig, ax = plt.subplots(subplot_kw={'projection': 'polar'}, figsize = (10, 8))
ax.set_theta_zero_location("N")
ax.set_rlabel_position(225)
for i in range(0, g.NPOP):
	LabelName = "{}".format(i)
	ax.plot(zenith_angles, gains[i], color = colors[i], linestyle = '--', alpha = 0.4, label = LabelName, linewidth = 2)

angle=np.deg2rad(0)
ax.legend(loc = 'lower left', bbox_to_anchor=(0.6 + np.cos(angle)/2, 0.25 + np.sin(angle)/2))
plt.title("Antennas at Frequency number {} MHz".format(round(83.33 + 16.67*(g.freq_num-1), 6)), fontsize = 16)
plt.savefig(g.destination + "/polar_plot_" + str(round(83.33 + 16.67 * (g.freq_num-1), 3)) + ".png")

