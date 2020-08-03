
########    Execute our initial genetic algorithm (A)    #############################################################################
#
#
#   This part of the loop  ::
#
#      1. Runs genetic algorithm
#
#
#      2. Moves GA outputs and renames the .csv file so it isn't overwritten 
#
#
#
#
#######################################################################################################################################
#variables
gen=$1
NPOP=$2
WorkingDir=$3
RunName=$4
GeoFactor=$5

#chmod -R 777 /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/

# NOTE: roulette_algorithm.exe should be compiled from roulette_algorithm_cut_test.cpp
# (It tells you at the top of the cpp file how to compile)

cd $WorkingDir
if [ $gen -eq 0 ]
then
	
	./roulette_algorithm.exe start $NPOP $GeoFactor 

else
	./roulette_algorithm.exe cont $NPOP $GeoFactor
fi

cp generationDNA.csv Run_Outputs/$RunName/${gen}_generationDNA.csv

#chmod -R 777 /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/
