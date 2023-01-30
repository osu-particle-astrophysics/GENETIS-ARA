#!/bin/bash
## This job is designed to be submitted by an array batch submission
## Here's the command:
## sbatch --array=1-NPOP*SEEDS%max --export=ALL,(variables) AraSimCall_Array.sh
#SBATCH -A PAS1960
#SBATCH -t 18:00:00
#SBATCH -N 1
#SBATCH -n 8
#SBATCH --output=/fs/ess/PAS1960/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build/XF_Loop/Evolutionary_Loop/Run_Outputs/%x/AraSim_Outputs/AraSim_%a.output
#SBATCH --error=/fs/ess/PAS1960/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build/XF_Loop/Evolutionary_Loop/Run_Outputs/%x/AraSim_Errors/AraSim_%a.error

#variables
#gen=$1
#WorkingDir=$2
#RunName=$3

#cd into the AraSim directory
#cd /fs/ess/PAS1960/BiconeEvolutionOSC/AraSim/

source /fs/ess/PAS1960/BiconeEvolutionOSC/new_root/new_root_setup.sh

cd $AraSimDir

#this is the command in the XF script although I don't know if we can pass in variables from that script
#into this one like i and WorkingDir
#if in the job call we have 

num=$(($((${SLURM_ARRAY_TASK_ID}-1))/${Seeds}+1))
seed=$(($((${SLURM_ARRAY_TASK_ID}-1))%${Seeds}+1))

echo a_${num}_${seed}.txt
#chmod -R 777 /fs/ess/PAS1960/BiconeEvolutionOSC/AraSim/outputs/
chmod -R 777 $AraSimDir/outputs/

./AraSim setup.txt ${SLURM_ARRAY_TASK_ID} $TMPDIR a_${num}.txt > $TMPDIR/AraOut_${gen}_${num}_${seed}.txt 
cd $TMPDIR
echo "Let's see what's in TMPDIR:"
ls -alrt 

mv AraOut.setup.txt.run${SLURM_ARRAY_TASK_ID}.root $WorkingDir/Antenna_Performance_Metric/AraOut_${gen}_${num}_${seed}.root
mv AraOut_${gen}_${num}_${seed}.txt $WorkingDir/Antenna_Performance_Metric/
## We can call Dennis' script here
#mv * $WorkingDir/Antenna_Performance_Metric

#cd $WorkingDir/Run_Outputs/$RunName/AraSimFlags
#echo ${num}_${Seeds} > ${num}_${Seeds}.txt
echo $gen > $TMPDIR/${num}_${seed}.txt
echo $num >> $TMPDIR/${num}_${seed}.txt
echo $seed >> $TMPDIR/${num}_${seed}.txt
cd $TMPDIR

echo "Let's see what's in TMPDIR:"
ls -alrt

mv ${num}_${seed}.txt $WorkingDir/Run_Outputs/$RunName/AraSimFlags

## This part appears unnecessary now
: << 'END'
# we need to go fix the file names from the jobs
# first, the AraOut Files
cd $WorkingDir/Antenna_Performance_Metric

## The below commands are only relevant when using pbsdcp
## They'll print an error to the error file, but they shouldn't really cause a problem
cp AraOut_${gen}_${num}_${seed}.txt.* AraOut_${gen}_${num}_${seed}.txt
rm AraOut_${gen}_${num}_${seed}.txt.*

# now do the flag files
cd $WorkingDir/Run_Outputs/$RunName/AraSimFlags

cp ${num}_${seed}.txt.* ${num}_${seed}.txt
rm ${num}_${seed}.txt.*
END
