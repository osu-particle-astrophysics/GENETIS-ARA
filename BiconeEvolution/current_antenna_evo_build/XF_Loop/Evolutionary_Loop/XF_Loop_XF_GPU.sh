#!/bin/bash
#This is a functionized version of the loop using savestates that also has seeded versions of AraSim
#Evolutionary loop for antennas.
#Last update: Feb 28, 2020 by Alex M. 
#OSU GENETIS Team
#PBS -e /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build/XF_Loops/Evolutionary_Loop/scriptEOFiles
#PBS -o /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build/XF_Loops/Evolutionary_Loop/scriptEOFiles
#
#This is a comment for Git Testing
################################################################################################################################################
#
#### THIS COPY SUBMITS XF SIMS AS A JOB AND REQUESTS A GPU FOR THE JOB ####
# This loop contains 7 different parts. Each part is its own function and is contained in its own bash script (Part_A to Part_F, with there being 2 part_Ds). When the loop is finished running through, it will restart for a set number of generations. 
# The code is optimised for a dynamic choice of NPOP UP TO fitnessFunction.exe. From there on, it has not been checked.
#
#
#
################################################################################################################################################


#make sure we're using python3
module load python/3.6-conda5.2

####### LINES TO CHECK OVER WHEN STARTING A NEW RUN ###############################################################################################

RunName='shorty_6_8'      ## This is the name of the run. You need to make a unique name each time you run.
TotalGens=2  			   ## number of generations (after initial) to run through
NPOP=2 		                   ## number of individuals per generation; please keep this value below 99
Seeds=10                            ## This is how many AraSim jobs will run for each individual
FREQ=60 			   ## the number frequencies being iterated over in XF (Currectly only affects the output.xmacro loop)
NNT=10000                           ## Number of Neutrinos Thrown in AraSim   
exp=18				   ## exponent of the energy for the neutrinos in AraSim
ScaleFactor=1.0                    ## ScaleFactor used when punishing fitness scores of antennae larger than the drilling holes
GeoFactor=1 			   ## This is the number by which we are scaling DOWN our antennas. This is passed to many files
num_keys=5			  ## how many XF keys we are letting this run use
database_flag=0   ## 0 if not using the database, 1 if using the database
#These next 3 define the symmetry of the cones.
SYMMETRY=0    	#If SYMMETRY=1, its a bicone. If SYMMETRY=0, its asymmetric		
LENGTH=1	#If 1, length is asymetric. If 0, length if symmetric
ANGLE=1		#If 1, angle is asymetric. If 0, angle is symmetric
NSECTIONS=2 	#The number of chromosomes

#####################################################################################################################################################

########  Initialization of variables  ###############################################################################################################
BEOSC=/fs/project/PAS0654/BiconeEvolutionOSC
WorkingDir=`pwd` ## this is where the loop is; on OSC this is /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build_XF_Loop/Evolutionary_Loop
echo $WorkingDir
XmacrosDir=$WorkingDir/../Xmacros
XFProj=$WorkingDir/Run_Outputs/${RunName}/${RunName}.xf  ## Provide path to the project directory in the 'single quotes'
echo $XFProj
#AraSimExec="/fs/project/PAS0654/BiconeEvolutionOSC/AraSim"  ##Location of AraSim.exe
AraSimExec="${WorkingDir}/../../../../AraSim"

AntennaRadii=$WorkingDir
##Source araenv.sh for AraSim libraries##
#source /fs/project/PAS0654/BiconeEvolutionOSC/araenv.sh
source $WorkingDir/../../../../araenv.sh
#####################################################################################################################################################

##Check if saveState exists and if not then create one at 0,0
saveStateFile="${RunName}.savestate.txt"

## We have a savestate that allows us to pick back up if we interrupt the loop ##
echo "${saveStateFile}"
cd saveStates
if ! [ -f "${saveStateFile}" ]; then
    echo "saveState does not exist. Making one and starting new run"
	
    echo 0 > $RunName.savestate.txt
    echo 0 >> $RunName.savestate.txt
    echo 1 >> $RunName.savestate.txt
fi
cd ..

## Read current state of loop ##
line=1
InititalGen=0
state=0
indiv=0
while read p; do
	if [ $line -eq 1 ]
	then
		InitialGen=$p 

	fi
	
	if [ $line -eq 2 ]
	then
		state=$p
	        
	fi
	
	if [ $line -eq 3 ]
	then
	        indiv=$p
		
	fi
	
	if [ $line -eq 2 ]
	then
		line=3

	fi

	if [ $line -eq 1 ]
	then
		line=2

	fi

       
	
done <saveStates/$saveStateFile ## this outputs to the state of the loop to the savestate file



## THE LOOP ##
echo "${InitialGen}"
echo "${state}"
echo "${indiv}"
#state=`echo ${state} | bc`
#InitialGen=${gen}

for gen in `seq $InitialGen $TotalGens`
do
	#read -p "Starting generation ${gen} at location ${state}. Press any key to continue... " -n1 -s
	

	## This only runs if starting new run ##
	if [[ $gen -eq 0 && $state -eq 0 ]]
	then
	        read -p "Starting generation ${gen} at location ${state}. Press any key to continue... " -n1 -s
		# Make the run name directory
		mkdir -m777 $WorkingDir/Run_Outputs/$RunName
		mkdir -m777 $WorkingDir/Run_Outputs/$RunName/AraSimFlags
		mkdir -m777 $WorkingDir/Run_Outputs/$RunName/GPUFlags
		mkdir -m777 $WorkingDir/Run_Outputs/$RunName/XFGPUOutputs
		# Create the run's date and save it in the run's directory
		python dateMaker.py
		mv "runDate.txt" "$WorkingDir/Run_Outputs/$RunName/" -f
		state=1
	fi




	## Part A ##
	##Here, we are running the genetic algorithm and moving the outputs to csv files 
	if [ $state -eq 1 ]
	then
		./Part_A_With_Switches.sh $gen $NPOP $WorkingDir $RunName $GeoFactor $SYMMETRY $LENGTH $ANGLE $NSECTIONS
		state=2
		./SaveState_Prototype.sh $gen $state $RunName $indiv
		#./Part_A.sh $gen $NPOP $WorkingDir $RunName


	fi


	## Part B1 ##
	if [ $state -eq 2 ]
	then

		if [ $database_flag -eq 0 ]
		then
			./Part_B_GPU_job1.sh $indiv $gen $NPOP $WorkingDir $RunName $XmacrosDir $XFProj $GeoFactor $num_keys

		else
			./Part_B_GPU_job1_database.sh $indiv $gen $NPOP $WorkingDir $RunName $XmacrosDir $XFProj $GeoFactor $num_keys

		fi
		state=3

		./SaveState_Prototype.sh $gen $state $RunName $indiv
		#./Part_B.sh $gen $NPOP $WorkingDir $RunName $XmacrosDir $XFProj
	fi

	## Part B2 ##
	if [ $state -eq 3 ]
	then

		if [ $database_flag -eq 0 ]
		then
		./Part_B_GPU_job2.sh $indiv $gen $NPOP $WorkingDir $RunName $XmacrosDir $XFProj $GeoFactor $num_keys

		else
		./Part_B_GPU_job2_database.sh $indiv $gen $NPOP $WorkingDir $RunName $XmacrosDir $XFProj $GeoFactor $num_keys
		fi

		state=4

		./SaveState_Prototype.sh $gen $state $RunName $indiv
		#./Part_B.sh $gen $NPOP $WorkingDir $RunName $XmacrosDir $XFProj
	fi

	## Part C ##
	if [ $state -eq 4 ]
	then
	  indiv=1
	  ./Part_C.sh $NPOP $WorkingDir $RunName $gen $indiv
		state=5

		./SaveState_Prototype.sh $gen $state $RunName $indiv
		#./Part_C.sh $NPOP $WorkingDir


	fi

	## Part D1 ##
	if [ $state -eq 5 ]
	then
	        #The reason here why Part_D1.sh is run after teh save state is changed is because all Part_D1 does is submit AraSim jobs which are their own jobs and run on their own time
		#We need to make a new AraSim job script which takes the runname as a flag 
		./Part_D1_AraSeed.sh $gen $NPOP $WorkingDir $AraSimExec $exp $NNT $RunName $Seeds
		state=6

		./SaveState_Prototype.sh $gen $state $RunName $indiv

	fi

	## Part D2 ##
	if [ $state -eq 6 ]
	then
	  ./Part_D2_AraSeed.sh $gen $NPOP $WorkingDir $RunName $Seeds
		state=7
		./SaveState_Prototype.sh $gen $state $RunName $indiv
		#./Part_D2.sh $gen $NPOP $WorkingDir $RunName


	fi

	## Part E ##
	## Concatenates the AraSim data files into a string so that it's usable for getting scores
	## Gets important information on the fitness scores and generation DNA
	## moves the .uan files from Antenna Performance Metric to RunOutputs/$RunName folder
	if [ $state -eq 7 ]
	then
	   ./Part_E_AraSeed.sh $gen $NPOP $WorkingDir $RunName $ScaleFactor $AntennaRadii $indiv $Seeds $GeoFactor $NSECTIONS
		state=8
		./SaveState_Prototype.sh $gen $state $RunName $indiv 
		#./Part_E.sh $gen $NPOP $WorkingDir $RunName $ScaleFactor $AntennaRadii

	fi

	## Part F ##
	if [ $state -eq 8 ]
	then
	  ./Part_F.sh $NPOP $WorkingDir $RunName $gen $Seeds
		state=1
		./SaveState_Prototype.sh $gen $state $RunName $indiv

		#./Part_F.sh $NPOP $WorkingDir $RunName


	fi
done

cp generationDNA.csv "$WorkingDir"/Run_Outputs/$RunName/FinalGenerationParameters.csv
mv runData.csv Antenna_Performance_Metric

#########################################################################################################################
###Moving the Veff AraSim output for the actual ARA bicone into the $RunName directory so this data isn't lost in     ###
###the next time we start a run. Note that we don't move it earlier since (1) our plotting software and fitness score ###
###calculator expect it where it is created in "$WorkingDir"/Antenna_Performance_Metric, and (2) we are only creating ###
###it once on gen 0 so it's not written over in the looping process.                                                  ###
########################################################################################################################
cd "$WorkingDir"
mv AraOut_ActualBicone.txt "$WorkingDir"/Run_Outputs/$RunName/AraOut_ActualBicone.txt


