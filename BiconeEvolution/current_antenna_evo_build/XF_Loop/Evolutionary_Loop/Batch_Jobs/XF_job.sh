#!/bin/bash

## the purpose of this script is to submit a job for a run where the first 4
## XF jobs didn't run, without having to redo the simulation design with the GUI

WorkingDir=/fs/ess/PAS1960/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build/XF_Loop/Evolutionary_Loop
RunName='Machtay_20201016_Symmetric_Improved_GA'
XmacrosDir=$WorkingDir/../Xmacros
XFProj=$WorkingDir/Run_Outputs/${RunName}/${RunName}.xf
NPOP=50

cd $WorkingDir

for i in `seq 501 504`
do

	individual_number=$i
	indiv_dir_parent=$XFProj/Simulations/000$individual_number/
	indiv_dir=$indiv_dir_parent/Run0001
	m=$i

	sbatch -N 1 -n 40 -G 2 --mem-per-gpu=178gb -t 0:10:00 -A PAS1960 --export=ALL,WorkingDir=$WorkingDir,RunName=$RunName,XmacrosDir=$XmacrosDir,XFProj=$XFProj,NPOP=$NPOP,indiv=$individual_number,indiv_dir=$indiv_dir,m=$m --job-name=XF_GPU_job_${individual_number}.run GPU_XF_Job.sh

done
