
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
ANGLE=$9
SEPARATION=${10}
NSECTIONS=${11}

#chmod -R 777 /fs/ess/PAS1960/BiconeEvolutionOSC/BiconeEvolution/

# NOTE: the asymmetric bicone_GA.exe should be compiled from fourGeneGA_cutoff_testing.cpp
# (It tells you at the top of the cpp file how to compile)

cd $WorkingDir

# if the bcione is symmetric, we have a different algorithm to run

if [ $NSECTIONS -eq 1 ] # if $NSECTIONS is 1, then it is symmetric (see Asym_XF_Loop.sh)
then
	#g++ -std=c++11 roulette_algorithm_cut_test.cpp -o bicone_GA.exe 
	g++ -std=c++11 GA/Algorithms/improved_GA.cpp -o GA/Executables/bicone_GA.exe
	if [ $gen -eq 0 ]
	then
		./GA/Executables/bicone_GA.exe start $NPOP $GeoFactor
	else
		./GA/Executables/bicone_GA.exe cont $NPOP $GeoFactor 
	fi	
else
	#g++ -std=c++11 fourGeneGA_cutoff_testing.cpp -o bicone_GA.exe
	#g++ -std=c++11 Asym_identical_starts.cpp -o bicone_GA.exe
	#g++ -std=c++11 Asym_GA_latest_version_identical_starts.cpp -o bicone_GA.exe
	g++ -std=c++11 GA/Algorithms/Latest_Asym_GA.cpp -o GA/Executables/bicone_GA.exe
	if [ $gen -eq 0 ]
	then
		#./GA/Executables/bicone_GA.exe start $NPOP $GeoFactor 
		./GA/Executables/bicone_GA.exe start $NPOP $GeoFactor 3 36 8 #$NSECTIONS $GeoFactor #$RADIUS $LENGTH $ANGLE $SEPARATION
	else
		#./GA/Executables/bicone_GA.exe start $NPOP $GeoFactor
		./GA/Executables/bicone_GA.exe cont $NPOP $GeoFactor 3 36 8 #$NSECTIONS $GeoFactor #$RADIUS $LENGTH $ANGLE $SEPARATION 
	fi
fi

echo "Flag: Successfully Ran GA!"

cp generationDNA.csv Run_Outputs/$RunName/${gen}_generationDNA.csv
mv generationDNA.csv $WorkingDir/Generation_Data/
mv generators.csv $WorkingDir/Generation_Data/
if [ $gen -gt 0 ]
then
	mv parents.csv $WorkingDir/Generation_Data/
	mv genes.csv $WorkingDir/Generation_Data/
	mv mutations.csv $WorkingDir/Generation_Data/
fi

#chmod -R 777 /fs/ess/PAS1960/BiconeEvolutionOSC/BiconeEvolution/

