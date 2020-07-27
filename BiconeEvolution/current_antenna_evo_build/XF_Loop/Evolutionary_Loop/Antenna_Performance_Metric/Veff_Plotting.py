import numpy as np		
import matplotlib.pyplot as plt	
import os			
import argparse
import csv

## Last Revision: Machtay, 7/8/20
#
#
## The purpose of this program is to plot the effective volumes from AraSim in the GENETIS loop.
#
#
## This program requires five arguments to run:
### 1. source -- where the effective volume data is held (usually the run name directory)
### 2. destination -- where to save the plot (usually the run name directory)
### 3. numGens -- how many generations to plot (usually as many as are available)
### 4. NPOP -- how many individuals to plot (usually as many as exist per generation)
### 5. Seeds -- the number of AraSim jobs per individual (usually 10, but variable in the loop)
#
#
## To run this code, use the following (with python 3 loaded):
## python /path/to/Veff_Plotting.py /path/to/<Run_Name> /path/to/<Run_Name> numGens NPOP Seeds
##
## For example (from the working directory):
## python Antenna_Performance_Metric/Veff_Plotting.py Run_Outputs/Length_Cutoff_Test_3 Run_Outputs/Length_Cutoff_Test_3 8 8 10

parser = argparse.ArgumentParser()
parser.add_argument("source", help="Name of source folder from home directory", type=str)
parser.add_argument("destination", help="Name of destination folder from home directory", type=str)
parser.add_argument("numGens", help="Number of generations the code is running for", type=int)
parser.add_argument("NPOP", help="Number of individuals in a population", type=int)
parser.add_argument("Seeds", help="The seed for AraSim", type=int)
g = parser.parse_args()

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

"""
#getting the veffective for the evolved antennas
for ind in range(1,g.NPOP+1):
#going to change from for gen in range(g.numgens) to for gen in range(0, g.numgens)
    for gen in range(0, g.numGens+1):
        filename = "AraOut_{}_{}.txt".format(gen, ind,)
#        print(filename)
 #       print(g.source)
        fp = open(g.source + "/" + filename, "r")
        #fp = open(g.source + "/" + filename)
        #line = fp.readlines()
        #print(line)
        for line in fp:
            if "test Veff(ice) : " in line:
                Veff = float(line.split()[5]) #changed from 3 to use units of km^3sr instead of m^3sr
            elif "And Veff(water eq.) error plus :" in line:
                Err_plus = float(line.split()[6])
                Err_minus = float(line.split()[11])
#            line = fp.readline()
        fp.close()
        tempVeff.append(Veff)
        tempErr_plus.append(Err_plus)
        tempErr_minus.append(Err_minus)
    VeffArray.append(tempVeff)
    Err_plusArray.append(tempErr_plus)
    Err_minusArray.append(tempErr_minus)
    tempVeff = []
    tempErr_plus = []
    tempErr_minus = []
#fp.close()
"""
#new way of getting veffs of evolved antennas

#we need to loop over the individuals
for ind in range(1, g.NPOP+1):
    lineNum = ind + 1 #the line in the csv files that the individual data is in 
    #we need to loop over all the generations, since the gen is in the file names
    for gen in range(0, g.numGens+1):
        #we need to give the changeable filenames we're gonna read
        veffs = "{}_vEffectives.csv".format(gen)
        errors = "{}_errorBars.csv".format(gen)
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
		


#Getting veffective for the actual ARA bciones
filenameActual = "AraOut_ActualBicone.txt"
fpActual = open(g.source + "/" + filenameActual)
for line in fpActual:
    if "test Veff(ice) : " in line:
        Veff_ARA = float(line.split()[5]) #changed from 3 to use units of km^3sr instead of m^3sr
    elif "And Veff(water eq.) error plus :" in line:
        Err_plus_ARA = float(line.split()[6])
        Err_minus_ARA = float(line.split()[11])
#    line = fpActual.readline()
fpActual.close()

genAxis = np.linspace(0,g.numGens,g.numGens+1, endpoint=True)

#print(genAxis)
#print(Veff_ARA)

#Veff_ARA_Ref = Veff_ARA * np.ones(len(genAxis))
plt.figure(figsize = (10, 8))
#plt.plot(genAxis, Veff_ARA_Ref, label = "ARA Reference", marker = 'o', color = 'k')
plt.axhline(y=Veff_ARA, linestyle = '--', color = 'k')
plt.axes([-1, g.numGens+1, -1, 6])
plt.xlabel('Generation', size = 21)
plt.ylabel('Length [cm]')
plt.ylabel('Fitness Score (Ice Volume) (km^3sr)', size = 21)
plt.title("Veff over Generations (0 - {})".format(int(g.numGens)), size = 23)
#plt.legend()

ax = plt.subplot(111)
ax.set_ylim(bottom = -0.2, top = max(max(VeffArray)) + max(max(Err_plusArray)) + 0.5)
for ind in range(0, g.NPOP):
    LabelName = "{}".format(ind+1)
    yerr_plus = Err_plusArray[ind]
    yerr_minus = Err_minusArray[ind]
    #ax.xlabel('Generation', size = 21)
    #ax.ylabel('Fitness Score (Ice Volume) ($km^3$sr)', size = 21)
    #ax.title('Generation', size = 21)
    plt.errorbar(genAxis, VeffArray[ind], yerr = [yerr_minus, yerr_plus], label = LabelName, marker = 'o', linestyle = '', alpha=0.4, markersize = 18)
  
#plt.axes([-1, g.numGens+1, -1, 6])
#plt.xlabel('Generation', size = 21)
#plt.ylabel('Length [cm]')
#plt.ylabel('Fitness Score (Ice Volume) (km^3sr)', size = 21)
#plt.title("Veff over Generations (0 - {})".format(int(g.numGens)), size = 23)
#plt.legend()


plt.xlabel('Generation', size = 26)
plt.ylabel('V\u03A9$_{eff}$ (km$^3$str)', size = 26)
plt.title("V\u03A9$_e$$_f$$_f$ over Generations (0 - {})".format(int(g.numGens)), size = 30)
#plt.legend()

plt.savefig(g.destination + "/" + "Veff_plot.png")
plt.show(block=False)
plt.pause(10)
