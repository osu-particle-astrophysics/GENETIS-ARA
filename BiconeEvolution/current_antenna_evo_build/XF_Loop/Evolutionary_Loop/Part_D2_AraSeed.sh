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
rm -f $WorkingDir/Run_Outputs/$RunName/AraSimFlags/*
#rm -f $WorkingDir/Run_Outputs/AraSimFlags/*

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

rm $WorkingDir/Run_Outputs/$RunName/AraSimFlags/*
#rm $WorkingDir/Run_Outputs/AraSimFlags/*
#file check delay goes here

wait

cd "$WorkingDir"/Antenna_Performance_Metric
#saving AraSim outputs so they are not overwritten.
#This following was used before TMPDIR was used for AraOut transfer
#for i in `seq 1 $NPOP`
#do
#    for j in `seq 1 $Seeds`
#    do
#    	cp AraOut_"${i}"_"${j}".txt $WorkingDir/Run_Outputs/$RunName/AraOut_"${gen}"_"${i}"_"${j}".txt
#    done
#done

if [ $gen -eq 0 ]
then
	cp $WorkingDir/Antenna_Performance_Metric/AraOut_ActualBicone.txt $WorkingDir/Run_Outputs/$RunName/AraOut_ActualBicone.txt
fi



#chmod -R 777 /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/

