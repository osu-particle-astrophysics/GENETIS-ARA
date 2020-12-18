#!/bin/bash

########    XF Simulation Software (B)     ########################################################################################## 
#
#
#     1. Prepares output.xmacro with generic parameters such as :: 
#             I. Antenna type
#             II. Population number
#             III. Grid size
#
#
#     2. Prepares simulation_PEC.xmacro with information such as:
#             I. Each generation antenna parameters
#
#
#     3. Runs XF and loads XF with both xmacros. 
#
#
###################################################################################################################################### 
# varaibles
indiv=$1
gen=$2
NPOP=$3
WorkingDir=$4
RunName=$5
XmacrosDir=$6
XFProj=$7
GeoFactor=$8
num_keys=$9


#chmod -R 777 /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/

module load xfdtd/7.8.1.4

## Lines for output.xmacro files ##
## I've commented these out because we needed to put them inside of a loop in the macroskeleton ##
## Currently these are hardcoded outputmacroskeleton_GPU.xmacro ##
#line1='var query = new ResultQuery();'
#line2='///////////////////////Get Theta and Phi Gain///////////////'
#line3='query.projectId = App.getActiveProject().getProjectDirectory();'
freqlist="8333 10000 11667 13333 15000 16667 18334 20000 21667 23334 25000 26667 28334 30000 31667 33334 35000 36667 38334 40001 41667 43334 45001 46667 48334 50001 51668 53334 55001 56668 58334 60001 61668 63334 65001 66668 68335 70001 71668 73335 75001 76668 78335 80001 81668 83335 85002 86668 88335 90002 91668 93335 95002 96668 98335 100000 101670 103340 105000 106670"
#The list of frequencies, scaled up by 100 to avoid float operation errors in bash
#we have to wait to change the frequencies since we're going to be changing them as we append them to simulation_PEC.xmacro (which is removed below before being remade)


cd $WorkingDir/Run_Outputs/$RunName/GPUFlags/
#start by checking that we have the jobs from the first batch
flag_files=0

if [ $NPOP -lt $num_keys ]
then
	batch_size=$NPOP
else
	batch_size=$num_keys
fi

#To do this, we need to wait until the number of flag files is 3 (by checking there are 4 files)
while [[ $flag_files -le $batch_size ]]
do
	echo "Waiting for the first batch of jobs to complete"
	sleep 60
	flag_files=$(ls -l --file-type | grep -v '/$' | wc -l)
done

#Now we need to repeat that for the rest of the jobs
totPop=$(echo $NPOP+1 | bc) #this is what flag_files goes up to
echo $totPop
while [[ $flag_files -lt $totPop ]] #we need to loop until flag_files reaches totPop
do
	#we want to send as many jobs as we can--num_keys
	#but we have to make sure we don't keep sending more til they finish
	remaining_jobs=$(echo $totPop-$flag_files | bc)
	if [[ $remaining_jobs -ge $num_keys ]]
	then
		next_jobs=$(echo $flag_files+$num_keys-1 | bc)
	#we need to account for the fact that sometimes we have fewer jobs than keys; it's bad to oversend jobs
	else
		next_jobs=$(echo $totPop-1 | bc)
	fi

	for m in `seq $flag_files $next_jobs`
	do

		individual_number=$(($gen*$NPOP + $m))

		cd $WorkingDir
		if [ $individual_number -lt 10 ]
		then
			indiv_dir=$XFProj/Simulations/00000$individual_number/Run0001/
		elif [[ $individual_number -ge 10  &&  $individual_number -lt 100 ]]
		then
			indiv_dir=$XFProj/Simulations/0000$individual_number/Run0001/
		elif [[ $individual_number -ge 100 && $individual_number -lt 1000 ]]
		then
			indiv_dir=$XFProj/Simulations/000$individual_number/Run0001/
		elif [ $individual_number -ge 1000 ]
		then
			indiv_dir=$XFProj/Simulations/00$individual_number/Run0001/
		fi

		output_location=$WorkingDir/scriptEOFiles
		sbatch -N 1 -n 40 -G 2 --mem-per-gpu=178gb -t 3:00:00 -A PAS0654 --export=ALL,WorkingDir=$WorkingDir,RunName=$RunName,XmacrosDir=$XmacrosDir,XFProj=$XFProj,NPOP=$NPOP,indiv=$individual_number,indiv_dir=$indiv_dir,m=$m --job-name=XF_GPU_job_${individual_number}.run GPU_XF_Job.sh

	done
	
	cd $WorkingDir/Run_Outputs/$RunName/GPUFlags/
	flag_files=$(ls -l --file-type | grep -v '/$' | wc -l)
	while [[ $flag_files -le $next_jobs ]]
	do
		echo "Waiting for this batch of jobs to finish..."
		sleep 60
		flag_files=$(ls -l --file-type | grep -v '/$' | wc -l)
		batch_remaining=$(echo $next_jobs-$flag_files+1 | bc)
		echo "Remaining jobs in this batch:"
		echo $batch_remaining
	done
done

rm -f $WorkingDir/Run_Outputs/$RunName/GPUFlags/*

echo $flag_files
echo "Done!"

# First, remove the old .xmacro files
#when do that, we end up making the files only readable; we should just overwrite them
#alternatively, we can just set them as rwe when the script makes them
cd $XmacrosDir
 
rm output.xmacro

#echo "var m = $i;" >> output.xmacro
echo "var NPOP = $NPOP;" >> output.xmacro
echo "for (var k = $(($gen*$NPOP + 1)); k <= $(($gen*$NPOP+$NPOP)); k++){" >> output.xmacro

cat shortened_outputmacroskeleton.txt >> output.xmacro

sed -i "s+fileDirectory+${WorkingDir}+" output.xmacro
# When we use the sed command, anything can be the delimiter between each of the arguments; usually, we use /, but since there are / in the thing we are trying to substitute in ($WorkingDir), we need to use a different delimiter that doesn't appear there


module load xfdtd
xfdtd $XFProj --execute-macro-script=$XmacrosDir/output.xmacro || true --splash=false

cd $WorkingDir/Antenna_Performance_Metric
for i in `seq $(($gen*$NPOP + $indiv)) $(($gen*$NPOP+$NPOP))`
do
	pop_ind_num=$(($i - $gen*$NPOP))
	for freq in `seq 1 60`
	do
		mv ${i}_${freq}.uan "$WorkingDir"/Run_Outputs/$RunName/${gen}_${pop_ind_num}_${freq}.uan
	done
done


#chmod -R 777 /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/

