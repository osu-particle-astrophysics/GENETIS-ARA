#!/bin/bash
## This job is designed to be submitted by an array batch submission
## Here's the command:
## sbatch --array=1-NPOP*SEEDS%max --export=ALL,(variables) AraSimCall_Array.sh
#SBATCH -A PAS1960
#SBATCH -t 4:00:00
#SBATCH -N 1
#SBATCH -n 40
#SBATCH --output=/fs/ess/PAS1960/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build/XF_Loop/Evolutionary_Loop/Run_Outputs/%x/AraSim_Outputs/AraSim_%a.output
#SBATCH --error=/fs/ess/PAS1960/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build/XF_Loop/Evolutionary_Loop/Run_Outputs/%x/AraSim_Errors/AraSim_%a.error

#variables
#gen=$1
#WorkingDir=$2
#RunName=$3

cd $AraSimDir

source /fs/ess/PAS1960/BiconeEvolutionOSC/new_root/new_root_setup.sh

threads=40
num=$(($((${SLURM_ARRAY_TASK_ID}-1))/${Seeds}+1))
seed=$(($((${SLURM_ARRAY_TASK_ID}-1))%${Seeds}+1))
# init = seed - 1
init=$((${seed}-1))

echo a_${num}_${seed}.txt

chmod -R 777 $AraSimDir/outputs/

# starts running $threads processes of AraSim
echo "Starting AraSim processes"
for (( i=0; i<${threads}; i++ ))
do
    # we need $threads unique id's for each seed
    indiv_thread=$((${init}*${threads}+${i}))
    echo "individual thread is $indiv_thread"
    ./AraSim setup.txt ${indiv_thread} $TMPDIR a_${num}.txt > $TMPDIR/AraOut_${gen}_${num}_${indiv_thread}.txt &
done

wait

cd $TMPDIR

echo "Done running AraSim processes"

echo "Let's see what's in TMPDIR:"
ls -alrt

echo "Moving AraSim outputs to final destination"
for (( i=0; i<${threads}; i++ ))
do
    indiv_thread=$((${init}*${threads}+${i}))
    echo "individual thread is $indiv_thread"
    #mv AraOut.setup.txt.run${indiv_thread}.root $WorkingDir/Antenna_Performance_Metric/AraOut_${gen}_${num}_${indiv_thread}.root
    rm AraOut.setup.txt.run${indiv_thread}.root
    mv AraOut_${gen}_${num}_${indiv_thread}.txt $WorkingDir/Antenna_Performance_Metric/AraOut_${gen}_${num}_${indiv_thread}.txt
done

wait

echo $gen > $TMPDIR/${num}_${seed}.txt
echo $num >> $TMPDIR/${num}_${seed}.txt
echo $seed >> $TMPDIR/${num}_${seed}.txt
cd $TMPDIR

echo "Let's see what's in TMPDIR:"
ls -alrt

mv ${num}_${seed}.txt $WorkingDir/Run_Outputs/$RunName/AraSimFlags
