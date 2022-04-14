
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

#chmod -R 777 /fs/ess/PAS1960/BiconeEvolutionOSC/BiconeEvolution/

# NOTE: roulette_algorithm.exe should be compiled from roulette_algorithm_cut_test.cpp
# (It tells you at the top of the cpp file how to compile)

cd $WorkingDir
if [ $gen -eq 0 ]
then
	g++ -std=c++11 GA/Algorithms/roulette_algorithm.cpp -o GA/Executables/roulette_algorithm.exe	
	./GA/Executables/roulette_algorithm.exe start $NPOP $GeoFactor 

else
	g++ -std=c++11 GA/Algorithms/roulette_algorithm.cpp -o GA/Executables/roulette_algorithm.exe
	./GA/Executables/roulette_algorithm.exe cont $NPOP $GeoFactor
fi

cp Generation_Data/generationDNA.csv Run_Outputs/$RunName/${gen}_generationDNA.csv
mv Generation_Data/generators.csv Run_Outputs/$RunName/${gen}_generators.csv
if [ $gen -gt 0 ]
then
        mv Generation_Data/parents.csv Run_Outputs/$RunName/${gen}_parents.csv
        mv Generation_Data/genes.csv Run_Outputs/$RunName/${gen}_genes.csv
        mv Generation_Data/mutations.csv Run_Outputs/$RunName/${gen}_mutations.csv
fi


#chmod -R 777 /fs/ess/PAS1960/BiconeEvolutionOSC/BiconeEvolution/
