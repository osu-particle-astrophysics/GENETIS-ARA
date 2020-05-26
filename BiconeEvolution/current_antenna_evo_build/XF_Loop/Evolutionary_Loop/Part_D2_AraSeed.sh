#This is a version of Part_D2 that uses multiple Seeds for an individual run of AraSim

#variables
gen=$1
NPOP=$2
WorkingDir=$3
RunName=$4
Seeds=$5

#chmod -R 777 /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/

cd $WorkingDir/Run_Outputs/$RunName/AraSimFlags/
#cd $WorkingDir/Run_Outputs/AraSimFlags/
nFiles=0

totPop=$( expr $NPOP \* $Seeds )
if [ $gen -eq 0 ]
then
	totPop=$( expr $NPOP \* $Seeds + 1 )
fi

while [ "$nFiles" != "$totPop" ]
do
	echo "Waiting for AraSim jobs to finish..."
	sleep 60
	nFiles=$(ls -1 --file-type | grep -v '/$' | wc -l)

#	echo "Waiting for AraSim jobs to finish..."
#	sleep 60
#	shopt -s nullglobs
#	numfiles=(*)
#	numfiles=${#numfiles[@]}
#	nFiles=$numfiles
#	#nFiles=$(ls -1 --file-type | grep -v '/$' | wc -l)
done

rm -f $WorkingDir/Run_Outputs/$RunName/AraSimFlags/*

wait

cd "$WorkingDir"/Antenna_Performance_Metric


if [ $gen -eq 100 ]
then
	cp $WorkingDir/Antenna_Performance_Metric/AraOut_ActualBicone.txt $WorkingDir/Run_Outputs/$RunName/AraOut_ActualBicone.txt
fi



#chmod -R 777 /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/

