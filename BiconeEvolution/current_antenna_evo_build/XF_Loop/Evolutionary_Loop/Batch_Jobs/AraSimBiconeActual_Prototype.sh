#!/bin/bash
#SBATCH -A PAS1960
#SBATCH -t 10:00:00
#SBATCH -N 1
#SBATCH -n 4
#SBATCH -o /fs/ess/PAS1960/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build/XF_Loop/Evolutionary_Loop/scriptEOFiles/
#SBATCH -e /fs/ess/PAS1960/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build/XF_Loop/Evolutionary_Loop/scriptEOFiles/
#cd into the AraSim directory
cd /fs/ess/PAS1960/BiconeEvolutionOSC/AraSim/
#cd $AraSimDir
#this is the command in the XF script although I don't know if we can pass in variables from that script
#into this one like i and WorkingDir
#if in the job call we have

##WorkingDir=$1
##RunName=$2

source /fs/ess/PAS1960/BiconeEvolutionOSC/new_root/new_root_setup.sh

#echo /fs/ess/PAS1960/BiconeEvolutionOSC/AraSim/ARA_bicone6in_output.txt
#echo $AraSimDir/ARA_bicone6in_output.txt

#./AraSim setup.txt 1 outputs/ /fs/ess/PAS1960/BiconeEvolutionOSC/AraSim/ARA_bicone6in_output.txt > /fs/ess/PAS1960/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build/XF_Loop/Evolutionary_Loop/Antenna_Performance_Metric/AraOut_ActualBicone.txt

#./AraSim setup_many_nus.txt 1 outputs/ $AraSimDir/ARA_bicone6in_output.txt > $TMPDIR/AraOut_Actual_Bicone_10_18_Updated.txt
./AraSim SETUP/setup_many_nus.txt 1 outputs/ ARA_bicone6in_output.txt > $TMPDIR/AraOut_Actual_Bicone_10_18_Updated_2.txt

cd $TMPDIR


pbsdcp -g * ~

#cd $WorkingDir/Run_Outputs/$RunName/AraSimFlags

#cd /fs/ess/PAS1960/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build/XF_Loop/Evolutionary_Loop/AraSimFlags/
#echo ARABicone > ARABicone.txt
#echo ARABicone_${Seeds} > ARABicone_${num}_${Seeds}.txt
