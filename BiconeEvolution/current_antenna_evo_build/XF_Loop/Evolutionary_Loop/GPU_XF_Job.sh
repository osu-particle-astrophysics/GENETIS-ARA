## We want to submit XF as a job to a GPU
#PBS -o /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build/XF_Loop/Evolutionary_Loop/scriptEOFiles/
#PBS -e /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build/XF_Loop/Evolutionary_Loop/scriptEOFiles/

# varaibles
#WorkingDir=$1
#RunName=$2
#XmacrosDir=$3 
#XFProj=$4 
#NPOP=$5 
#indiv=$6

## make sure we're in the right directory
cd $WorkingDir
cd Run_Outputs/$RunName/GPUFlags

module load xfdtd
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
xfsolver --use-xstream=true --xstream-use-number=1 --num-threads=1 -v

cd $WorkingDir
cd Run_Outputs/$RunName/GPUFlags

echo "The GPU job is done!" >> Part_B_GPU_Flag_$m.txt 
