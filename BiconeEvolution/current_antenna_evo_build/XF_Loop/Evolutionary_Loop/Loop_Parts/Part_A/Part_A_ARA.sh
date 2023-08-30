############### Execute our initial genetic algorithm #####################
#
#
#   This part of the Loop:
#
#   1. Runs the genetic algorithm
#
#   2. Moved GA outputs and renames the .csv file so it isn't overwritten
#
#
############################################################################
#variables
gen=$1
NPOP=$2
WorkingDir=$3
RunName=$4
GeoFactor=$5
rank_no=$6
roulette_no=$7
tournament_no=$8
reproduction_no=$9
crossover_no=${10}
mutationRate=${11}
sigma=${12}

cd $WorkingDir

g++ -std=c++11 GA/Shared-Code/GA/SourceFiles/New_GA.cpp -o GA/New_GA.exe
./GA/New_GA.exe ARA $gen $NPOP $rank_no $roulette_no $tournament_no $reproduction_no $crossover_no $mutationRate $sigma

mkdir -m775 $WorkingDir/Run_Outputs/$RunName/Generation_Data/Generation_${gen}
cp Generation_Data/generationDNA.csv Run_Outputs/$RunName/Generation_Data/Generation_${gen}/${gen}_generationDNA.csv
mv Generation_Data/generators.csv Run_Outputs/$RunName/Generation_Data/$Generation_${gen}/{gen}_generators.csv

if [ $gen -gt 0 ]
then
	mv Generation_Data/parents.csv Run_Outputs/$RunName/Generation_Data/$Generation_${gen}/{gen}_parents.csv
fi
chmod -R 775 Generation_Data/
