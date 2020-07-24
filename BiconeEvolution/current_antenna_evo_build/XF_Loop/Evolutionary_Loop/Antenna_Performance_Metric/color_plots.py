import numpy as np
import matplotlib.pyplot as plt
from matplotlib.lines import Line2D
import csv
import argparse 

# run with python3 color_plots.py /path/to/runName /path/to/runName(NPOP) (gens) (seeds)

parser = argparse.ArgumentParser();
parser.add_argument("source", type=str)
parser.add_argument("destination", type=str)
parser.add_argument("NPOP", type=int)
parser.add_argument("numGens", type=int)
parser.add_argument("Seeds", type=int)
g = parser.parse_args()


# let's get the Veffective data
Veff = []
Err_plus = []
Err_minus = []
VeffArray = []
Err_plusArray = []
Err_minusArray = []

tempVeff = []
tempErr_plus = []
tempErr_minus = []

Veff_ARA = []
Err_plus_ARA = []
Err_minus_ARA = []
Veff_ARA_Ref = []

#we need to loop over the individuals
for gen in range(0, g.numGens+1):
	#we need to loop over all the generations, since the gen is in the file names
	veffs = "{}_vEffectives.csv".format(gen)
	errors = "{}_errorBars.csv".format(gen)
	for ind in range(1, g.NPOP+1):
		lineNum = ind + 1 #the line in the csv files that the individual data is in 
		#we need to give the changeable filenames we're gonna read
		#veffs = "{}_vEffectives.csv".format(gen)
		#errors = "{}_errorBars.csv".format(gen)
			#for each generation, we need to get all the veffs and error bars
		with open(g.source + "/" + veffs, "r") as vr: #vr for veff read
			v_read = csv.reader(vr, delimiter=',') #reading vr as a csv
			for i, row in enumerate(v_read): #loop over the rows 
				if i == lineNum: #skipping the header
					Veff = float(row[0]) #lineNum contains veff
		vr.close()
		with open(g.source + "/" + errors, "r") as er: #er for errors read 
			e_read = csv.reader(er, delimiter=',')
			for i, row in enumerate(e_read):
				if i == lineNum:
					Err_plus = float(row[0])
					Err_minus = float(row[1])
		er.close()
		#fill the generation individual values into arrays to hold them temporarily
		tempVeff.append(Veff)
		tempErr_plus.append(Err_plus)
		tempErr_minus.append(Err_minus)
	#The temporary files contain the same individual at different generations
	#we want to store these now in the arrays containing all the data
	VeffArray.append(tempVeff)
	Err_plusArray.append(tempErr_plus)
	Err_minusArray.append(tempErr_minus)
	tempVeff = []
	tempErr_plus = []
	tempErr_minus = []
		
#print(VeffArray)
#print(Err_minusArray)
#print(Err_plusArray)

print(np.shape(VeffArray))
print(np.shape(Err_minusArray))
print(np.shape(Err_plusArray))

# let's get the fitness score data

tempFitnesses = []
FitnessesArray = []
# read the files
for gen in range(0, g.numGens+1):
	#we need to loop over all the generations, since the gen is in the file names
	fitnesses = "{}_fitnessScores.csv".format(gen)
	for ind in range(1, g.NPOP+1):
		lineNum = ind + 1 #the line in the csv files that the individual data is in 
	#we need to give the changeable filenames we're gonna read
		#for each generation, we need to get all the fitnesses
		with open(g.source + "/" + fitnesses, "r") as fr: #fr for fitnesses read
			f_read = csv.reader(fr, delimiter=',') #reading fr as a csv
			for i, row in enumerate(f_read): #loop over the rows 
				if i == lineNum: #skipping the header
					fitness = float(row[0]) #lineNum contains the fitness score
		fr.close()
		#fill the generation individual values into arrays to hold them temporarily
		tempFitnesses.append(fitness)
#The temporary files contain the same individual at different generations
#we want to store these now in the arrays containing all the data
	FitnessesArray.append(tempFitnesses)
	tempFitnesses = []


#Let's plot!


# The fitness score plot

fscores_plot = plt.figure(figsize=(10, 8))
plt.axis([-0.5, g.numGens + 0.5, -0.5, np.amax(FitnessesArray)+0.5])
genAxis = np.linspace(0,g.numGens,g.numGens+1,endpoint=True)

# Here's the ARA bicone data
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

plt.axhline(y=Veff_ARA, linestyle = '--', color = 'k')
#plt.axhline(y=(Veff_ARA + Err_plus_ARA), linestyle = '--', color = 'r')
#plt.axhline(y=(Veff_ARA - Err_minus_ARA), linestyle = '--', color = 'r')


for gen in range(g.numGens+1):
	#LabelName = "Individual {}".format(ind+1)
	plt.xlabel('Generation', size = 26)
	plt.ylabel('Fitness Score (km$^3$str)', size = 26)
	plt.title("Fitness Score over Generations (0 - {})".format(int(g.numGens)), size = 30)
	for ind in range(g.NPOP):		
		if VeffArray[gen][ind] == FitnessesArray[gen][ind]:
			c = 'g'
			#LabelName = "Unpenalized"
		else:
			c = 'r'
			#LabelName = "Penalized"
		plt.plot(gen, FitnessesArray[gen][ind], marker = 'o', color = c, linestyle='', alpha = 0.4, markersize = 12)

legend_elements = [Line2D([0], [0], marker = 'o', color='w', label='Unpenalized', markerfacecolor='g', markersize=12), Line2D([0], [0], marker='o', color='w', label='Penalized', markerfacecolor='r', markersize=12), Line2D([0], [0], color='k', label='ARA Bicone', markerfacecolor='w', linestyle = '--')]

plt.xticks(np.arange(0, g.numGens + 1, step=1), fontsize = 16)
plt.yticks(np.arange(0, 6, step=1), fontsize = 16)
#plt.legend(handles=legend_elements, loc='right', fontsize = 18)





# The Veffective plot

Veff_plot = plt.figure(figsize = (10, 8))
plt.axis([-0.5, g.numGens + 0.5, -0.5, np.amax(VeffArray)+0.5])
plt.axhline(y=Veff_ARA, linestyle = '--', color = 'k')

for gen in range(g.numGens+1):
	#LabelName = "Individual {}".format(ind+1)
	plt.xlabel('Generation', size = 26)
	plt.ylabel('V\u03A9$_{eff}$ (km$^3$str)', size = 26)
	plt.title("V\u03A9$_e$$_f$$_f$ over Generations (0 - {})".format(int(g.numGens)), size = 30)
	for ind in range(g.NPOP):		
		if VeffArray[gen][ind] == FitnessesArray[gen][ind]:
			c = 'g'
			LabelName = "Unpenalized"
		else:
			c = 'r'
			LabelName = "Penalized"
		#plt.plot(gen, VeffArray[gen][ind], label = LabelName, marker = 'o', color = c, linestyle='', alpha = 0.4, markersize = 12)
		yerr_minus = Err_minusArray[gen][ind]
		yerr_plus = Err_plusArray[gen][ind]
		plt.errorbar(gen, VeffArray[gen][ind], yerr = yerr_plus, label = LabelName, marker = 'o', color = c, linestyle='', alpha = 0.4, markersize = 12) # all the plus and minus error bars are the same
		

legend_elements = [Line2D([0], [0], marker = 'o', color='w', label='Unpenalized', markerfacecolor='g', markersize=12), Line2D([0], [0], marker='o', color='w', label='Penalized', markerfacecolor='r', markersize=12), Line2D([0], [0], color='k', label='ARA Bicone', markerfacecolor='w', linestyle = '--')]

plt.xticks(np.arange(0, g.numGens + 1, step=1), fontsize = 16)
plt.yticks(np.arange(0, 6, step=1), fontsize = 16)
#plt.legend(handles=legend_elements, loc='right', fontsize = 18)
#plt.show()

fscores_plot.savefig(g.destination + "/" + "Fitness_Scores_RG.png")
Veff_plot.savefig(g.destination + "/" + "Veffectives_RG.png")
