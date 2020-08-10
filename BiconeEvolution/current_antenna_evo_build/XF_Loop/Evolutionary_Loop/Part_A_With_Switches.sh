
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
NSECTIONS=$3
WorkingDir=$4
#WorkingDir=/users/PAS0654/eliotaferstl/GENETISBicone/BiconeEvolution/current_antenna_evo_build/XF_Loop/Evolutionary_Loop
RunName=$5
GeoFactor=$6
RADIUS=$7
LENGTH=$8
ANGLE=$9
SEPARATION=${10}
NSECTIONS=${11}

#chmod -R 777 /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/

# NOTE: the asymmetric bicone_GA.exe should be compiled from fourGeneGA_cutoff_testing.cpp
# (It tells you at the top of the cpp file how to compile)

cd $WorkingDir

# if the bcione is symmetric, we have a different algorithm to run

if [ $NSECTIONS -eq 1 ] # if SYMMETRY is 1, then it is symmetric (see Asym_XF_Loop.sh)
then
	g++ -std=c++11 roulette_algorithm_cut_test.cpp -o bicone_GA.exe
else
	g++ -std=c++11 fourGeneGA_cutoff_testing.cpp -o bicone_GA.exe
fi


if [ $gen -eq 0 ]
then
	./bicone_GA.exe start $NPOP $NSECTIONS $GeoFactor $RADIUS $LENGTH $ANGLE $SEPARATION
else
	./bicone_GA.exe cont $NPOP $NSECTIONS $GeoFactor $RADIUS $LENGTH $ANGLE $SEPARATION
fi

echo "Flag: Successfully Ran GA!"

cp generationDNA.csv Run_Outputs/$RunName/${gen}_generationDNA.csv

#chmod -R 777 /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/

