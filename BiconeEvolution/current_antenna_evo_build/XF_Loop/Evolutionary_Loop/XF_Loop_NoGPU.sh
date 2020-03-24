#!/bin/bash
#This is a functionized version of the loop using savestates that also has seeded versions of AraSim
#Evolutionary loop for antennas.
#Last update: Feb 28, 2020 by Alex M
#OSU GENETIS Team
#PBS -e /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build/XF_Loops/Evolutionary_Loop/scriptEOFiles
#PBS -o /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build/XF_Loops/Evolutionary_Loop/scriptEOFiles

################################################################################################################################################
#
######THIS COPY IS USED TO SUBMIT XF SIMS AS A JOB USING ONLY A CPU. IT IS USED IF WE CANNOT SUCCESSFULLY GET A GPU#######
# This loop contains 7 different parts. Each part is its own function and is contained in its own bash script (Part_A to Part_F, with there being 2 part_Ds). When the loop is finished running 
# through, it will restart for a set number of generations. 
# The code is optimised for a dynamic choice of NPOP UP TO fitnessFunction.exe. From there on, it has not been checked.
#
#
################################################################################################################################################


#make sure we're using python3
module load python/3.6-conda5.2

####### LINES TO CHECK OVER WHEN STARTING A NEW RUN ###############################################################################################

RunName='RubyTest'      ## This is the name of the run. You need to make a unique name each time you run.
TotalGens=2 			   ## number of generations (after initial) to run through
NPOP=10 		                   ## number of individuals per generation; please keep this value below 99
Seeds=4                            ## This is how many AraSim jobs will run for each individual
FREQ=60 			   ## the number frequencies being iterated over in XF (Currectly only affects the output.xmacro loop)
NNT=10000                           ## Number of Neutrinos Thrown in AraSim   
exp=18				   ## exponent of the energy for the neutrinos in AraSim
ScaleFactor=1.0                    ## ScaleFactor used when punishing fitness scores of antennae larger than the drilling holes
GeoFactor=2 			   ## This is the number by which we are scaling DOWN our antennas. This is passed to many files

###########################New variables we need to pass ################
# To do this will involve changing the roulette algorithm somewhat drastically. I'll work on this in the meeting on 2/21/20 --Machtay
# The below variables are passed to the roulette algorithm to be used as the properties of the gaussians which generate the antennas
# Antenna Dimensions and their standard deviations (passed to roulette algorithm)
# Antenna_length = # the mean length of the antennas
# Antenna_length_std = # the standard deviation of the length of the antennas
# Antenna_radius = # the mean radius of the antennas
# Antenna_radius_std = # the standard deviation of the radius of the antennas
# Antenna_angle = # the opening angle of the bicone
# Antenna_angle_std = # the standard deviation of the opening angle of the bicone
# Setting these in the bash script means we need to recompile the roulette algorithm every time we run

#####################################################################################################################################################

########  Initialization of variables  ###############################################################################################################
BEOSC=/fs/project/PAS0654/BiconeEvolutionOSC
WorkingDir=`pwd` #this is /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build/XF_Loop/Evolutionary_Loop
echo $WorkingDir
XmacrosDir=$WorkingDir/../Xmacros
XFProj=$WorkingDir/Run_Outputs/${RunName}/${RunName}.xf  ## Provide path to the project directory in the 'single quotes'
echo $XFProj
AraSimExec="/fs/project/PAS0654/BiconeEvolutionOSC/AraSim"  ##Location of AraSim.exe
AntennaRadii=$WorkingDir
##Source araenv.sh for AraSim libraries##
source /fs/project/PAS0654/BiconeEvolutionOSC/araenv.sh
#####################################################################################################################################################

##Check if saveState exists and if not then create one at 0,0
saveStateFile="${RunName}.savestate.txt"

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
		#echo "${p}"
		#echo "${InitialGen}"
	       

	fi
	
	if [ $line -eq 2 ]
	then
		state=$p
		#echo "${p}"
		#echo "${state}"
	        
	fi
	
	if [ $line -eq 3 ]
	then
	        indiv=$p
		#echo "${p}"
		#echo "${indiv}"
		
	fi
	
	if [ $line -eq 2 ]
	then
		line=3

	fi

	if [ $line -eq 1 ]
	then
		line=2

	fi

       
	
done <saveStates/$saveStateFile



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
		# Create the run's date and save it in the run's directory
		python dateMaker.py
		mv "runDate.txt" "$WorkingDir/Run_Outputs/$RunName/" -f
		state=1
	fi




	## Part A ##
	##Here, we are running the genetic algorithm and moving the outputs to csv files 
	if [ $state -eq 1 ]
	then
	        ./Part_A.sh $gen $NPOP $WorkingDir $RunName $GeoFactor
		state=2
		./SaveState_Prototype.sh $gen $state $RunName $indiv
		#./Part_A.sh $gen $NPOP $WorkingDir $RunName


	fi


	## Part B ##
	if [ $state -eq 2 ]
	then
		for i in `seq $indiv $NPOP`
		do

	        	./Part_B_NoGPU.sh $gen $NPOP $WorkingDir $RunName $XmacrosDir $XFProj $i $GeoFactor
			if [ $i -ne $NPOP ]
			then
				state=2
			else
				state=3
			       
			fi
			./SaveState_Prototype.sh $gen $state $RunName $i
			#./Part_B.sh $gen $NPOP $WorkingDir $RunName $XmacrosDir $XFProj
		done
	fi

	

	## Part C ##
	if [ $state -eq 3 ]
	then
	        $indiv=1
	        ./Part_C.sh $NPOP $WorkingDir $RunName $gen $indiv
		state=4

		./SaveState_Prototype.sh $gen $state $RunName $indiv
		#./Part_C.sh $NPOP $WorkingDir


	fi

	## Part D1 ##
	if [ $state -eq 4 ]
	then
	        #The reason here why Part_D1.sh is run after teh save state is changed is because all Part_D1 does is submit AraSim jobs which are their own jobs and run on their own time
		#We need to make a new AraSim job script which takes the runname as a flag 
		state=5

		./SaveState_Prototype.sh $gen $state $RunName $indiv
		./Part_D1_AraSeed.sh $gen $NPOP $WorkingDir $AraSimExec $exp $NNT $RunName $Seeds

	fi

	## Part D2 ##
	if [ $state -eq 5 ]
	then
	        ./Part_D2_AraSeed.sh $gen $NPOP $WorkingDir $RunName $Seeds
		state=6
		./SaveState_Prototype.sh $gen $state $RunName $indiv
		#./Part_D2.sh $gen $NPOP $WorkingDir $RunName


	fi

	## Part E ##
	## Concatenates the AraSim data files into a string so that it's usable for getting scores
	## Gets important information on the fitness scores and generation DNA
	## moves the .uan files from Antenna Performance Metric to RunOutputs/$RunName folder
	if [ $state -eq 6 ]
	then
	        ./Part_E_AraSeed.sh $gen $NPOP $WorkingDir $RunName $ScaleFactor $AntennaRadii $indiv $Seeds
		state=7
		./SaveState_Prototype.sh $gen $state $RunName $indiv $GeoFactor
		#./Part_E.sh $gen $NPOP $WorkingDir $RunName $ScaleFactor $AntennaRadii

	fi

	## Part F ##
	if [ $state -eq 7 ]
	then
	        ./Part_F.sh $NPOP $WorkingDir $RunName $gen
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


