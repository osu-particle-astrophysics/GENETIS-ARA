#This version of the AraSimCall is made to implement multiple seeds for an individual run of AraSim
#In order to run AraSim in Parallel we will have to call a job for it by calling a script
#PBS -A PAS0654
#PBS -l walltime=2:00:00
#PBS -l nodes=1:ppn=4
#PBS -o /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build/XF_Loop/Evolutionary_Loop/scriptEOFiles/
#PBS -e /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build/XF_Loop/Evolutionary_Loop/scriptEOFiles/

#variables
#num=$1
#WorkingDir=$2
#RunName=$3

#cd into the AraSim directory
#cd /fs/project/PAS0654/BiconeEvolutionOSC/AraSim/

cd $AraSimDir

#this is the command in the XF script although I don't know if we can pass in variables from that script
#into this one like i and WorkingDir
#if in the job call we have 
#qsub -v num=$i
runNum=$( expr $num \* 10 + $Seeds )
echo a_$num_$Seeds.txt
#chmod -R 777 /fs/project/PAS0654/BiconeEvolutionOSC/AraSim/outputs/
chmod -R 777 $AraSimDir/outputs/
#./AraSim setup.txt $runNum outputs/ a_${num}.txt > /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build/XF_Loop/Evolutionary_Loop/Antenna_Performance_Metric/AraOut_${num}_${Seeds}.txt

#./AraSim setup.txt $runNum outputs/ a_${num}.txt > $WorkingDir/Antenna_Performance_Metric/AraOut_${num}_${Seeds}.txt



./AraSim setup.txt $runNum outputs/ a_${num}.txt > $TMPDIR/AraOut_${gen}_${num}_${Seeds}.txt
cd $TMPDIR
pbsdcp -g '*' $WorkingDir/Antenna_Performance_Metric #$WorkingDir/Run_Outputs/$RunName
#I temporarily changed the above because I couldn't change Part_E

#The above command could be our problem with AraSim failing; one possibility is to use the flag pbsdcp
#pbsdcp would allow us to copy from the local storage of each job to the working directory without I/O errors, which could be our problem right now since we have so many files being filled at the same time
#see documentation here: https://www.osc.edu/~troy/pbstools/man/pbsdcp
#my thinking is that we would change the outputs/ flag to be $TMPDIR, which is the temporary storage for the job
#then we would use the command pbsdcp -g to "gather" the output file and move it into the directory we want in OSC (Antenna_Performance_Metric)



#cd $WorkingDir/Run_Outputs/$RunName/AraSimFlags
#echo ${num}_${Seeds} > ${num}_${Seeds}.txt
echo ${num}_${Seeds} > $TMPDIR/${num}_${Seeds}.txt
cd $TMPDIR
pbsdcp -g ${num}_${Seeds}.txt $WorkingDir/Run_Outputs/$RunName/AraSimFlags
