import matplotlib.pyplot as plt
import numpy as np
from matplotlib.lines import Line2D
import csv
import argparse

# run with:
# python3 polar_plots.py (source) (destination) (frequency number) (NPOP) (generation)

# example (from inside ../Antenna_Performance_Metric
# python3 polar_plots.py ../Run_Outputs/Ryan_test_run3/ ../Run_Outputs/Ryan_test_run3/polar_plots/ 14 10 4

# here are the arguments
parser = argparse.ArgumentParser();
parser.add_argument("source", type=str)
parser.add_argument("destination", type=str)
parser.add_argument("freq", type=int)
parser.add_argument("NPOP", type=int)
parser.add_argument("gen", type=int)
g = parser.parse_args()
# we need to make some arrays

phi = []
gain_dB = []

# we want to loop over all the individuals

for ind in range(1, g.NPOP + 1):
	lineNum = 383 # this is the first line with data in it
	with open (g.source + str(g.gen) + "_" + str(ind) + "_" + str(g.freq) + ".uan") as uan:
		uan_read = csv.reader(uan, delimiter = " ") #the delimiter is (space) (tab) (space)...
		for i, row in enumerate(uan_read):
			if i >= lineNum and i < lineNum+73:
				phi.append(float(row[1]) *  np.pi / 180)
				gain_dB.append(float(row[3]))# + 300)
				#print(phi)
				#print(i)
	uan.close()
	
	tick_list = np.linspace(-300, np.max(gain_dB), 4)
	#print(phi)
	#print(gain_dB)

	# now let's plot

	color_cycle = ['C0', 'C1', 'C2', 'C3', 'C4', 'C5', 'C6', 'C7', 'C8', 'C9']

	fig = plt.figure(figsize = (10, 6))
	ax = plt.subplot(111, projection = 'polar')
	#plt.polar(phi, gain_dB, 'ro')
	labelName = "Individual {}".format(str(ind))
	ax.plot(phi, gain_dB, marker = 'o', linestyle = '', label = labelName, color = color_cycle[ind - 1])
	ax.set_rticks(tick_list)
	#ax.set_rticks([-300, np.max(gain_dB)])
	ax.set_title("Gain Plot at Theta = 25 degrees, Frequency = 300 MHz", va = 'bottom')
	fig.savefig(g.destination + "{}_{}_gain.png".format(str(g.gen), str(ind)))
	#plt.ylabel("Gain (dB) + 300", size = 26)
	#plt.theta("Polar Angle", size = 26)

	phi = []
	gain_dB = []



