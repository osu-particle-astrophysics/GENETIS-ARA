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

module load xfdtd

## Lines for output.xmacro files ##
## I've commented these out because we needed to put them inside of a loop in the macroskeleton ##
## Currently these are hardcoded outputmacroskeleton_GPU.xmacro ##
#line1='var query = new ResultQuery();'
#line2='///////////////////////Get Theta and Phi Gain///////////////'
#line3='query.projectId = App.getActiveProject().getProjectDirectory();'
freqlist="8333 10000 11667 13333 15000 16767 18334 20000 21667 23334 25000 26667 28334 30000 31667 33333 35000 36767 38334 40001 41667 43333 45001 46767 48334 50001 51668 53334 55001 56768 58334 60001 61668 63334 65001 66768 68334 70001 71667 73334 75001 76768 78334 80001 81668 83335 85002 86668 88335 90002 91668 93335 95002 96668 98335 100000 101670 103340 105000 106670"
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
		cd $WorkingDir
		if [ $m -lt 10 ]
		then
			indiv_dir=$XFProj/Simulations/00000$m/Run0001/
			qsub -l nodes=1:ppn=40:gpus=2:default -l walltime=1:15:00 -A PAS0654 -v WorkingDir=$WorkingDir,RunName=$RunName,XmacrosDir=$XmacrosDir,XFProj=$XFProj,NPOP=$NPOP,indiv=$m,indiv_dir=$indiv_dir,m=$m GPU_XF_Job.sh ## Here's our job that will do the xfsolver
		elif [[ $m -ge 10  &&  $m -lt 100 ]]
		then
			indiv_dir=$XFProj/Simulations/0000$m/Run0001/
			qsub -l nodes=1:ppn=40:gpus=1:default -l walltime=1:15:00 -A PAS0654 -v WorkingDir=$WorkingDir,RunName=$RunName,XmacrosDir=$XmacrosDir,XFProj=$XFProj,NPOP=$NPOP,indiv=$m,indiv_dir=$indiv_dir,m=$m GPU_XF_Job.sh ## Here's our job that will do the xfsolver
			xfsolver --use-xstream=true --xstream-use-number=1 --num-threads=1 -v
		elif [ $m -ge 100 ]
		then
			indiv_dir=$XFProj/Simulations/000$m/Run0001/
			qsub -l nodes=1:ppn=40:gpus=1:default -l walltime=1:15:00 -A PAS0654 -v WorkingDir=$WorkingDir,RunName=$RunName,XmacrosDir=$XmacrosDir,XFProj=$XFProj,NPOP=$NPOP,indiv=$m,indiv_dir=$indiv_dir,m=$m GPU_XF_Job.sh ## Here's our job that will do the xfsolver
			xfsolver --use-xstream=true --xstream-use-number=1 --num-threads=1 -v
		fi
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

echo $flag_files
echo "Done!"

# First, remove the old .xmacro files
#when do that, we end up making the files only readable; we should just overwrite them
#alternatively, we can just set them as rwe when the script makes them
cd $XmacrosDir
 
rm output.xmacro

#echo "var m = $i;" >> output.xmacro
echo "var NPOP = $NPOP;" >> output.xmacro
cat outputmacroskeleton_GPU.txt >> output.xmacro

sed -i "s+fileDirectory+${WorkingDir}+" output.xmacro
# When we use the sed command, anything can be the delimiter between each of the arguments; usually, we use /, but since there are / in the thing we are trying to substitute in ($WorkingDir), we need to use a different delimiter that doesn't appear there


module load xfdtd
xfdtd $XFProj --execute-macro-script=$XmacrosDir/output.xmacro || true --splash=false

cd $WorkingDir/Antenna_Performance_Metric
for i in `seq $indiv $NPOP`
do
	for freq in `seq 1 60`
	do
		mv ${i}_${freq}.uan "$WorkingDir"/Run_Outputs/$RunName/${gen}_${i}_${freq}.uan
	done
done


#chmod -R 777 /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/

