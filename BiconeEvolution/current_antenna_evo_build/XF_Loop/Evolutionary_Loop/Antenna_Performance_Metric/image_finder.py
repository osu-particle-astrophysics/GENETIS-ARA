## File name: image_finder.py
## Authors: Audrey Zinn (zinn.60@osu.edu), Bailey Stephens (stephens.761@osu.edu)
## Date: 12/06/2021
## Purpose:
##     This script is designed to read the fitness scores for the current generation
##     and select the antenna with the best, middle, and worst fitness scores to
##     save an image of
##
## Instructions:
##              To run, give the following arguments
##                      source directory, generation #
##
## Example:
##              python image_finder.py sourceDir 23
##                      This will find the best, middle, and worst individuals
##                      in generation 23 using data in the sourceDir directory.


## Imports
import argparse
import csv

## Arguments
parser = argparse.ArgumentParser()
parser.add_argument("source", help="Name of source folder from home directory", type=str)
parser.add_argument("gen", help="Generation number (ex:0)", type=int)
g=parser.parse_args()

## Declare an empty list that will hold the fitness scores 
fitness_scores = []

## First we need to open the csv file that contains the values of the fitness scores.
with open(g.source+'/'+str(g.gen)+"_fitnessScores.csv") as f:
    
    ### We then itereate over every row in the fitnessScores.csv file
    for row in f:
        
        ### Checks if the current row is a fitness score or not
        if row[0].isdigit():
            
            row = row.split(',')
            
            ### Each fitness score is appended to a list
            fitness_scores.append(float(row[0]))

### Makes a list of indices and zips it with the list of fitness scores
index_list = [i for i in range(len(fitness_scores))]
zipped_fitness = zip(fitness_scores, index_list)

### Sorts the fitness scores / indices pairs and separates the lists
sorted_scores = sorted(zipped_fitness)
tuples = zip(*sorted_scores)
sorted_fitness_scores, sorted_indices = [list(tuple) for tuple in  tuples]

### Stores the max, min, and middle indices from the above sorted list
max_index = sorted_indices[-1]
mid_index = sorted_indices[round(len(fitness_scores)/2)]
min_index = sorted_indices[0]

### Creates a temporary file that holds the index for the best-performing detector
f=open('temp_best.csv', 'w')
#f.write(str(max_index+50*g.gen))
f.write(str(max_index))
f.close()

### Creates a temporary file that holds the index for the middle-performing detector
f=open('temp_mid.csv', 'w')
#f.write(str(mid_index+50*g.gen))
f.write(str(mid_index))
f.close()

### Creates a temporary file that holds the index for the worst-performing detector
f=open('temp_worst.csv', 'w')
#f.write(str(min_index+50*g.gen))
f.write(str(min_index))
f.close()
