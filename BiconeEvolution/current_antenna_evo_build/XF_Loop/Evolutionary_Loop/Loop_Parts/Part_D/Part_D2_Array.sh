#This is a version of Part_D2 that uses multiple Seeds for an individual run of AraSim

#variables
gen=$1
NPOP=$2
WorkingDir=$3
RunName=$4
Seeds=$5
AraSimExec=$6

#chmod -R 777 /fs/ess/PAS1960/BiconeEvolutionOSC/BiconeEvolution/

cd $WorkingDir/Run_Outputs/$RunName/AraSimFlags/
#cd $WorkingDir/Run_Outputs/AraSimFlags/
nFiles=0

totPop=$( expr $NPOP \* $Seeds )

while [ "$nFiles" != "$totPop" ] # as long as we're not done with AraSim (increase $nFiles each time AraSim success)
do
	echo "Waiting for AraSim jobs to finish..."
	sleep 20
	# we need to base the counter off of the new flags
	# these are in the AraSimConfirmed directory
	nFiles=$(ls -1 --file-type ../AraSimConfirmed | grep -v '/$' | wc -l) # update nFiles 
	
	# I'm adding a second set of flags
	# The first set of flags indicates that the jobs finished
	# The second set indicates that the job was successful
	# If the job was unsuccessful, we'll read the flag file to know which job to resubmit
	for file in *
	do

		#echo $file

		# if there are not files, then it will look at the literal name given in the for loop (gen_*)
		# we need to exclude that, since it's not a real file
		if [ "$file" != "*" ] && [ "$file" != "" ] # both are necessary
		# We don't want it to think * in the for loop is an actual file
		# Also, I've found that it also checks the empty file name "" for some reason
		then
			current_generation=$(head -n 1 $file) # what gen (should be this one)
			current_individual=$(head -n 2 $file | tail -n 1) # which individual?
			current_seed=$(head -n 3 $file | tail -n 1) # which seed of the individual

			#current_file="${gen}_${current_individual}_${current_seed}"
			current_file="AraSim_$(($((${current_individual}-1))*${Seeds}+${current_seed}))"	

			# now we need to check the error file produced for that job
			# the error file is in Run_Name/${gen}_AraSim_Outputs

			#echo $current_file

			if grep "segmentation violation" ../AraSim_Errors/${current_file}.error || grep "DATA_LIKE_OUTPUT" ../AraSim_Errors/${current_file}.error || grep "CANCELLED" ../AraSim_Errors/${current_file}.error 
			then
				# we need to remove the output and error file associated with that
				# otherwise, this loop will keep seeing it and keep resubmitting
				rm -f ../AraSim_Errors/${current_file}.error
				rm -f ../AraSim_Outputs/${current_file}.output

				echo "segmentation violation/DATA_LIKE_OUTPUT/CANCELLED error!" 
				
				# now we can resubmit the job
				cd $WorkingDir
				output_name=/fs/ess/PAS1960/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build/XF_Loop/Evolutionary_Loop/Run_Outputs/$RunName/AraSim_Outputs/${current_file}.output
				error_name=/fs/ess/PAS1960/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build/XF_Loop/Evolutionary_Loop/Run_Outputs/$RunName/AraSim_Errors/${current_file}.error
				sbatch --export=ALL,gen=$gen,num=${current_individual},WorkingDir=$WorkingDir,RunName=$RunName,Seeds=${current_seed},AraSimDir=$AraSimExec --job-name=AraSimCall_AraSeed_$gen_${current_individual}_${current_seed}.run --output=$output_name --error=$error_name Batch_Jobs/AraSimCall_AraSeed.sh

				cd Run_Outputs/$RunName/AraSimFlags/

				# since we need to rerun, we need to remove the flag
				rm -f ${current_individual}_${current_seed}.txt

			else
				# we need to add the second flag to denote that all is well if there was not error
				if [ "$current_individual" != "" ] && [ "$current_seed" != "" ] 
				then
					echo "This individual succeeded" > ../AraSimConfirmed/${current_individual}_${current_seed}_confirmation.txt
				fi
			fi
		fi
	done

	# Parker's alert system will go here
	# As long as this while loop is still going, we haven't gotten
	#  the number of files we need to move on. But, if we have no more
	#  jobs, then we know that there was a novel error in AraSim which
	#  prevents this loop from moving on.


done

rm -f $WorkingDir/Run_Outputs/$RunName/AraSimFlags/*
rm -f $WorkingDir/Run_Outputs/$RunName/AraSimConfirmed/*
rm -f $WorkingDir/Run_Outputs/$RunName/AraSim_Outputs/*
rm -f $WorkingDir/Run_Outputs/$RunName/AraSim_Errors/*
wait

cd "$WorkingDir"/Antenna_Performance_Metric


if [ $gen -eq 10000 ]
then
	cp $WorkingDir/Antenna_Performance_Metric/AraOut_ActualBicone.txt $WorkingDir/Run_Outputs/$RunName/AraOut_ActualBicone.txt
fi



#chmod -R 777 /fs/ess/PAS1960/BiconeEvolutionOSC/BiconeEvolution/

