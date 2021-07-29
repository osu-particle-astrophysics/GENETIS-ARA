
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
	
	./GA/roulette_algorithm.exe start $NPOP $GeoFactor 

else
	./GA/roulette_algorithm.exe cont $NPOP $GeoFactor
fi

cp generationDNA.csv Run_Outputs/$RunName/${gen}_generationDNA.csv
mv generators.csv $WorkingDir/Generation_Data/
mv generators.csv $WorkingDir/Generation_Data/
if [ $gen -gt 0 ]
then
        mv parents.csv $WorkingDir/Generation_Data/
        mv genes.csv $WorkingDir/Generation_Data/
        mv mutations.csv $WorkingDir/Generation_Data/
fi


#chmod -R 777 /fs/ess/PAS1960/BiconeEvolutionOSC/BiconeEvolution/
