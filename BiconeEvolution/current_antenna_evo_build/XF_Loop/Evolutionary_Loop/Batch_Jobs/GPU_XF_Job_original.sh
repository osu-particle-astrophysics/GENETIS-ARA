#!/bin/bash
## We want to submit XF as a job to a GPU
## Run as follows:
## sbatch -N 1 -n 40 -G 2 --mem-per-gpu=178gb -t 0:30:00 -A PAS1960 --export=ALL,WorkingDir=$WorkingDir,RunName=$RunName,XmacrosDir=$XmacrosDir,XFProj=$XFProj,NPOP=$NPOP,indiv=$individual_number,indiv_dir=$indiv_dir,m=$m --job-name=XF_GPU_job_${individual_number}.run GPU_XF_Job.sh

## make sure we're in the right directory
cd $WorkingDir
cd Run_Outputs/$RunName/GPUFlags

module load xfdtd/7.8.1.4
module load cuda

## Below is the old way of doing this by doing all of the xfsolvers in one job
## Leave it in case we need to revert to it
#for m in `seq $indiv $NPOP`
#do
#	if [ $m -lt 10 ]
#	then
#		cd $XFProj/Simulations/00000$m/Run0001/
#		#xfsolver -t=35 -v #--use-xstream #xstream
#		xfsolver --use-xstream=true --xstream-use-number=1 --num-threads=1 -v
#	elif [ $m -ge 10 ] && [ $m -lt 100]
#	then
#		cd $XFProj/Simulations/0000$m/Run0001/
#		#xfsolver -t=35 -v #--use-xstream #xstream
#		xfsolver --use-xstream=true --xstream-use-number=1 --num-threads=1 -v
#	elif [ $m -ge 100 ]
#	then
#		cd $XFProj/Simulations/000$m/Run0001/
#		#xfsolver -t=35 -v #--use-xstream #xstream
#		xfsolver --use-xstream=true --xstream-use-number=1 --num-threads=1 -v
#
#
#	fi
#done

cd $indiv_dir
xfsolver --use-xstream=true --xstream-use-number=2 --num-threads=2 -v

cd $WorkingDir
cd Run_Outputs/$RunName/GPUFlags

echo "The GPU job is done!" >> Part_B_GPU_Flag_${indiv}.txt 
