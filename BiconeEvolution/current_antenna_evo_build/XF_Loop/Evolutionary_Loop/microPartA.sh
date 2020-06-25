
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
#WorkingDir=/users/PAS0654/eliotaferstl/GENETISBicone/BiconeEvolution/current_antenna_evo_build/XF_Loop/Evolutionary_Loop
RunName=$4
GeoFactor=$5
SYMMETRY=$6
LENGTH=$7
ANGLE=$8
NSECTIONS=$9

echo "Flag: Beginning Part A"
#chmod -R 777 /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/

cd $WorkingDir
if [ $gen -eq 0 ]
then

	echo "Flag: Running GA Initial Generation"
	./rouletteWithSwitches.exe start $NPOP $GeoFactor $SYMMETRY $LENGTH $ANGLE $NSECTIONS
	echo "Flag: Finished GA Initial Generation"

else

	echo "Flag: Running GA First Generation"
	./rouletteWithSwitches.exe cont $NPOP $GeoFactor $SYMMETRY $LENGTH $ANGLE $NSECTIONS
	echo "Flag: Finished GA First Generation"

fi

echo "Flag: Successfully Ran GA!"

cp generationDNA.csv Run_Outputs/$RunName/${gen}_generationDNA.csv

#chmod -R 777 /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/

