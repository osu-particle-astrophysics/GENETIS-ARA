#!/bin/bash
#This version of the AraSimCall is made to implement multiple seeds for an individual run of AraSim
#In order to run AraSim in Parallel we will have to call a job for it by calling a script
#SBATCH -A PAS0654
#SBATCH -t 3:30:00
#SBATCH -N 1
#SBATCH -n 4
###SBATCH -o /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build/XF_Loop/Evolutionary_Loop/Run_Outputs/$RunName/${gen}_AraSim_Outputs/${gen}_${num}_${Seeds}.output
###SBATCH -e /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build/XF_Loop/Evolutionary_Loop/Run_Outputs/$RunName/${gen}_AraSim_Errors/${gen}_${num}_${Seeds}.error

#variables
#num=$1
#WorkingDir=$2
#RunName=$3

#cd into the AraSim directory
#cd /fs/project/PAS0654/BiconeEvolutionOSC/AraSim/

source /fs/project/PAS0654/BiconeEvolutionOSC/new_root/new_root_setup.sh

cd $AraSimDir

#this is the command in the XF script although I don't know if we can pass in variables from that script
#into this one like i and WorkingDir
#if in the job call we have 
#sbatch -v num=$i
runNum=$( expr $num \* 10 + $Seeds )
echo a_${num}_${Seeds}.txt
#chmod -R 777 /fs/project/PAS0654/BiconeEvolutionOSC/AraSim/outputs/
chmod -R 777 $AraSimDir/outputs/
#./AraSim setup.txt $runNum outputs/ a_${num}.txt > /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build/XF_Loop/Evolutionary_Loop/Antenna_Performance_Metric/AraOut_${num}_${Seeds}.txt

#./AraSim setup.txt $runNum outputs/ a_${num}.txt > $WorkingDir/Antenna_Performance_Metric/AraOut_${num}_${Seeds}.txt



./AraSim setup.txt $runNum outputs/ a_${num}.txt > $TMPDIR/AraOut_${gen}_${num}_${Seeds}.txt 
cd $TMPDIR
# potential alternative to pbsdcp
#sgather -r '*' $WorkingDir/Antenna_Performance_Metric

pbsdcp -g * $WorkingDir/Antenna_Performance_Metric #$WorkingDir/Run_Outputs/$RunName

#mv * $WorkingDir/Antenna_Performance_Metric

#I temporarily changed the above because I couldn't change Part_E

#The above command could be our problem with AraSim failing; one possibility is to use the flag pbsdcp
#pbsdcp would allow us to copy from the local storage of each job to the working directory without I/O errors, which could be our problem right now since we have so many files being filled at the same time
#see documentation here: https://www.osc.edu/~troy/pbstools/man/pbsdcp
#my thinking is that we would change the outputs/ flag to be $TMPDIR, which is the temporary storage for the job
#then we would use the command pbsdcp -g to "gather" the output file and move it into the directory we want in OSC (Antenna_Performance_Metric)



#cd $WorkingDir/Run_Outputs/$RunName/AraSimFlags
#echo ${num}_${Seeds} > ${num}_${Seeds}.txt
echo $gen > $TMPDIR/${num}_${Seeds}.txt
echo $num >> $TMPDIR/${num}_${Seeds}.txt
echo $Seeds >> $TMPDIR/${num}_${Seeds}.txt
cd $TMPDIR
# commented out; potential alternative to pbsdcp
#sgather ${num}_${Seeds}.txt $WorkingDir/Run_Outputs/$RunName/AraSimFlags/${num}_${Seeds}.txt

pbsdcp -g ${num}_${Seeds}.txt $WorkingDir/Run_Outputs/$RunName/AraSimFlags

# we need to go fix the file names from the jobs
# first, the AraOut Files
cd $WorkingDir/Antenna_Performance_Metric

cp AraOut_${gen}_${num}_${Seeds}.txt.* AraOut_${gen}_${num}_${Seeds}.txt
rm AraOut_${gen}_${num}_${Seeds}.txt.*

# now do the flag files
cd $WorkingDir/Run_Outputs/$RunName/AraSimFlags

cp ${num}_${Seeds}.txt.* ${num}_${Seeds}.txt
rm ${num}_${Seeds}.txt.*

