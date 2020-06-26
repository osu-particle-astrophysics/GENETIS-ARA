## File name: negative_gains.py
## Author: Alex Machtay (machtay.1@osu.edu)
## Purpose:
#### The goal of this file is to search the XF output files for negative gains
#### negative gains don't make sense and can cause 0 effective volumes for individuals
#### which shouldn't have zero effective volume
## Run this script as follows:
### python3 negative_gains.py (source) (individual number)
## For example:
### python3 negative_gains.py ~/../pattonalexo/AraSimReadFiles 7

# imports

import numpy as np
import csv
import argparse
import matplotlib.pyplot as plt
from statistics import mean

# arguments

parser = argparse.ArgumentParser()
parser.add_argument('source', type=str)
parser.add_argument('ind', type=int)
g = parser.parse_args()

# define the negative counter
negative_counter = 0


with open(g.source + "/a_" + str(g.ind) + ".txt") as f1:
	txt_read = csv.reader(f1, delimiter = " ") #the delimiter is a space
	for i, row in enumerate(txt_read):
		if row[1] != "Theta" and row[0] != "freq" and row[0] != "SWR":
			if float(row[12]) < 0:
				negative_counter += 1

f1.close()

print(f1)
print(negative_counter)
