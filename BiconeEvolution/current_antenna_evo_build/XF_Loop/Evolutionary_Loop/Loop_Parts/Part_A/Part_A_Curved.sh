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
RunName=$5
GeoFactor=$6
RADIUS=$7
LENGTH=$8
A=$9
B=${10}
SEPARATION=${11}
NSECTIONS=${12}

#chmod -R 777 /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/

# NOTE: the asymmetric bicone_GA.exe should be compiled from fourGeneGA_cutoff_testing.cpp
# (It tells you at the top of the cpp file how to compile)

cd $WorkingDir

# if the bcione is symmetric, we have a different algorithm to run

if [ $NSECTIONS -eq 1 ] # if SYMMETRY is 1, then it is symmetric (see Asym_XF_Loop.sh)
then
	g++ -std=c++11 GA/Algorithms/improved_GA.cpp -o GA/Executables/bicone_GA.exe
	if [ $gen -eq 0 ]
	then
		./GA/Executables/bicone_GA.exe start $NPOP $GeoFactor
	else
		./GA/Executables/bicone_GA.exe cont $NPOP $GeoFactor
	fi	
else
	#g++ -std=c++11 GA/Algorithms/Curved_GA.cpp -o GA/Executables/bicone_GA.exe
	#g++ -std=c++11 GA/Algorithms/Curved_GA_Constant_Quadratic.cpp -o GA/Executables/bicone_GA.exe
	g++ -std=c++11 GA/Algorithms/Elite_GA.cpp -o GA/Executables/bicone_GA.exe
	if [ $gen -eq 0 ]
	then
		# format:
		# ./a.out start/cont $NPOP $GeoFactor $Reproduction_no $Crossover_no $Roulette_fraction*10 $Tournament_fraction*10 $Rank_fraction*10
		#./GA/Executables/bicone_GA.exe start $NPOP $GeoFactor 3 36 8 2 0
		./GA/Executables/bicone_GA.exe start $NPOP 3 36 8 2 0 1
	else
		#./GA/Executables/bicone_GA.exe cont $NPOP $GeoFactor 3 36 8 2 0
		./GA/Executables/bicone_GA.exe cont $NPOP 3 36 8 2 0 1
	fi
fi

echo "Flag: Successfully Ran GA!"

cp generationDNA.csv $WorkingDir/Run_Outputs/$RunName/${gen}_generationDNA.csv
mv generationDNA.csv $WorkingDir/Generation_Data/
mv generators.csv $WorkingDir/Generation_Data/
if [ $gen -gt 0 ]
then
        mv parents.csv $WorkingDir/Generation_Data/
        mv genes.csv $WorkingDir/Generation_Data/
        mv mutations.csv $WorkingDir/Generation_Data/
fi
#chmod -R 777 /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/
