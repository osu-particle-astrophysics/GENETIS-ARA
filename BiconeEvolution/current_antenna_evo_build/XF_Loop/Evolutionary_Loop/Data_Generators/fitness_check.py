import csv
import math
import argparse
import numpy as np


parser = argparse.ArgumentParser();
parser.add_argument("Gen", type=int)
parser.add_argument("source", type=str)
g = parser.parse_args()

## fitness related arrays
A = [] ## fitness scores for current gen
B = [] ## fitness scores for previous gen
Err_A = [] ## error in A
Err_B = [] ## error in B


## DNA related arrays
Ar1 = []
Ar2 = []
Al1 = []
Al2 = []
At1 = []
At2 = []
Aq1 = []
Aq2 = []

Br1 = []
Br2 = []
Bl1 = []
Bl2 = []
Bt1 = []
Bt2 = []
Bq1 = []
Bq2 = []

## gather fitness scores and error from current Gen
with open(g.source + str(g.Gen) + "_fitnessScores.csv") as f1:
    txt_read = csv.reader(f1, delimiter = ',')
    for i, row in enumerate(txt_read):
        if i>1:
            A.append(float(row[0]))
            Err_A.append(float(row[1]))
f1.close()

## Gather fitness scores and error from previous Gen
with open(g.source + str(g.Gen-1) + "_fitnessScores.csv") as f2:
    txt_read = csv.reader(f2, delimiter = ',')
    for i, row in enumerate(txt_read):
        if i>1:
            B.append(float(row[0]))
            Err_B.append(float(row[1]))
f2.close()

## Read in values from current Gen DNA
with open("generationDNA.csv") as f3:
    csv_read = csv.reader(f3, delimiter = ',')
    for i, row in enumerate(csv_read):
        if i>8:
            if( i%2 != 0):
                Ar1.append(float(row[0]))
                Al1.append(float(row[1]))
                At1.append(float(row[2]))
                Aq1.append(float(row[3]))
            if( i%2 == 0):
                Ar2.append(float(row[0]))
                Al2.append(float(row[1]))
                At2.append(float(row[2]))
                Aq2.append(float(row[3]))
f3.close()
## Read in values from previous gen DNA
with open(g.source + str(g.Gen-1) + "_generationDNA.csv") as f4:
    csv_read = csv.reader(f4, delimiter = ',')
    for i, row in enumerate(csv_read):
        if i>8:
            if( i%2 !=0):
                Br1.append(float(row[0]))
                Bl1.append(float(row[1]))
                Bt1.append(float(row[2]))
                Bq1.append(float(row[3]))
            if( i%2 == 0):
                Br2.append(float(row[0]))
                Bl2.append(float(row[1]))
                Bt2.append(float(row[2]))
                Bq2.append(float(row[3]))
f4.close()


## combine fitness scores of repeated antennas and save them in the A arrays
for i in range(0, len(A)):
    for j in range(0, len(A)):
        if(Ar1[i] == Br1[j] and Ar2[i] == Br2[j] and Al1[i] == Bl1[j] and Al2[i] == Bl2[j] and At1[i] == Bt1[j] and At2[i] == Bt2[j] and Aq1[i] == Bq1[j] and Aq2[i] == Bq2[j]):
            if (Err_A[i] == 0.0):
                WA = 0.0
            else:
                WA = 1/(Err_A[i]**2)
            if (Err_B[j] == 0.0):
                WB = 0.0
            else:
                WB = 1/(Err_B[j]**2)
            A[i] = ((WA*A[i] + WB*B[j])/(WA+WB))
            Err_A[i] = 1/((WA + WB)**(1/2))

# write out to file 

with open('fitnessScores.csv', "r") as f5:
    lines = f5.readlines()
f5.close()

lines2 = []
with open(str(g.Gen)+'_fitnessScores.csv', 'w') as f6:
    for x in range(0, len(A)+2):
        if x<=1:
            lines2.append(str(lines[x]))
        elif x>1:
            lines2.append(str(A[x-2]) + "," +str(Err_A[x-2]) + '\n')
    f6.writelines(lines2)
f6.close()

    
