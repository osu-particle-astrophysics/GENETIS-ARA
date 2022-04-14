#!/bin/bash
#
## Files name: image_maker.sh
## Authors: Bailey Stephens (stephens.761), Audrey Zinn
## Date: 2/5/2022
## Purpose: 
##      This script serves two functions: run a Python script that identifies
##      the best-, middle-, and worst-performing detectors in a generation, and
##      and saves those photos into a labelled folder.
##
## Instructions:
##          To run, give the following arguments:
##                  .uan source directory, photo source directory, destination directory, generation #
##
## Example:
##          ./image_maker.sh fitnessSourceDir photoSourceDir destinationDir 23
##                  This will use fitness scores for generation 23 from fitnessSourceDir
##                  and save the best-, middle-, and worst-performing detector
##                  photos from photoSourceDir in destinationDir
#
fitnessSourceDir=$1
photoSourceDir=$2
destinationDir=$3
gen=$4
echo $fitnessSourceDir
echo $photoSourceDir
echo $destinationDir
#
### Creates a temporary file to hold the index of the best detector
touch temp_best.csv
touch temp_mid.csv
touch temp_worst.csv
#
### Runs a Python script that identifies the index of the best detector
python3 image_finder.py $fitnessSourceDir $gen
#
### Stores the indices of the best, middle, and worst individuals in variables
max_index=`cat temp_best.csv`
mid_index=`cat temp_mid.csv`
min_index=`cat temp_worst.csv`
#
### Makes directory for this generation's photos, then moves the indicated photos into it
mkdir ${destinationDir}/${gen}_detector_photos
mv ${photoSourceDir}/${max_index}_detector.png ${destinationDir}/${gen}_detector_photos/${gen}_${max_index}_detector_max.png
mv ${photoSourceDir}/${mid_index}_detector.png ${destinationDir}/${gen}_detector_photos/${gen}_${mid_index}_detector_mid.png
mv ${photoSourceDir}/${min_index}_detector.png ${destinationDir}/${gen}_detector_photos/${gen}_${min_index}_detector_min.png
#
### Removes the remaining photos
rm ${photoSourceDir}/*_detector.png
#
python $WorkingDir/Antenna_Performance_Metric/polar_plotter_v2.py $WorkingDir/Run_Outputs/$RunName/${gen}_Gain_Plots $RunName 13 $NPOP $gen
#
### Removes temporary files
rm temp_best.csv
rm temp_mid.csv
rm temp_worst.csv
