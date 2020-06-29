
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

echo "Flag: Beginning Part A"
#chmod -R 777 /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/

cd $WorkingDir
if [ $gen -eq 0 ]
then

	echo "Flag: Running GA Initial Generation"
	./rouletteWithSwitches.exe start $NPOP $NSECTIONS $GeoFactor $RADIUS $LENGTH $ANGLE
	echo "Flag: Finished GA Initial Generation"

else

	echo "Flag: Running GA First Generation"
	./rouletteWithSwitches.exe cont $NPOP $NSECTIONS $GeoFactor $RADIUS $LENGTH $ANGLE
	echo "Flag: Finished GA First Generation"

fi

echo "Flag: Successfully Ran GA!"

cp generationDNA.csv Run_Outputs/$RunName/${gen}_generationDNA.csv

#chmod -R 777 /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/

