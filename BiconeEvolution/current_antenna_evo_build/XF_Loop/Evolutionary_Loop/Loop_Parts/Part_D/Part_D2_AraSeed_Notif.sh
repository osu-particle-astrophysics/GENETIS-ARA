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
	sleep 60
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

			current_file="${gen}_${current_individual}_${current_seed}"
		

			# now we need to check the error file produced for that job
			# the error file is in Run_Name/${gen}_AraSim_Outputs

			#echo $current_file

			if grep "segmentation violation" ../${gen}_AraSim_Errors/${current_file}.error || grep "DATA_LIKE_OUTPUT" ../${gen}_AraSim_Errors/${current_file}.error 
			then
				# we need to remove the output and error file associated with that
				# otherwise, this loop will keep seeing it and keep resubmitting
				rm -f ../${gen}_AraSim_Errors/${current_file}.error
				rm -f ../${gen}_AraSim_Outputs/${current_file}.output

				# format of flags files:
				#  ${gen}_${individual}_${seed}.txt
				#  We should be able to count down the job_list.txt file to find which
				#   individual needs to be replaced
	
				# now we can resubmit the job
				cd $WorkingDir
				output_name=/fs/ess/PAS1960/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build/XF_Loop/Evolutionary_Loop/Run_Outputs/$RunName/${gen}_AraSim_Outputs/${gen}_${current_individual}_${current_seed}.output
				error_name=/fs/ess/PAS1960/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build/XF_Loop/Evolutionary_Loop/Run_Outputs/$RunName/${gen}_AraSim_Errors/${gen}_${current_individual}_${current_seed}.error
				jobscript=$(sbatch --export=ALL,gen=$gen,num=${current_individual},WorkingDir=$WorkingDir,RunName=$RunName,Seeds=${current_seed},AraSimDir=$AraSimExec --job-name=AraSimCall_AraSeed_$gen_${current_individual}_${current_seed}.run --output=$output_name --error=$error_name Batch_Jobs/AraSimCall_AraSeed.sh)

				jobID=$(echo "${jobscript}" | tr -d -c 0-9) 

				# while read job_list.txt;
				# We need to readd the job list file and fine where the failed jobID is
				# Then we want to replace that failed jobID with the new jobID
				# To find the failed jobID, just count through the file til you get to
				#  the line number corresponding to the job (current_indiv - 1)*Seeds + current_seed
				line_no=1
				while IFS= read -r line
				do

					if [ $line_no -eq $(((current_individual-1)*(Seeds)+current_seed)) ]
					then
						replace_string=$line
					fi			
				line_no=$((line_no + 1))
				done < $WorkingDir/job_list.txt


				sed -i "s/${replace_string}/${jobID}/g" $WorkingDir/job_list.txt

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

	joblist=()
	# let's get the list of jobs
	while IFS= read -r line
	do
        	joblist+=( $line )
	done < $WorkingDir/job_list.txt

	## Let's get the number of jobs remaining
	JH_counter=0

	for(( i=0; i<$totPop; i++ ))
        do
                jstatus=$(squeue -j ${joblist[i]})
                if grep -q "${joblist[i]}" <<< "$jstatus"
                then
                        ((JH_counter++))
                fi
        done
	
	## Let's get the number of flag files that have been made
	## ERROR 2/1/21 MACHTAY LOOK HERE
	## For some reason, this command touches a file with the number numflagsconf
	## It also complains that it can't acces the directory, despite doing this
	numflagsconf=$(ls $WorkingDir/Run_Outputs/$RunName/AraSimConfirmed/ | wc -l)
	if [ $(( numflagsconf + JH_counter )) -eq $totPop ]
	then
		echo "Flags and jobs add up correctly."
		# all is well
		# does this need a wait timer? think ab it
	elif [ $(( numflagsconf + JH_counter )) -gt $totPop ]
	then
		echo "A job may have been resubmitted, but it should be okay."
		# something is going on but i think it's all good still
		# need anything here?
	elif [ $(( numflagsconf + JH_counter )) -lt $totPop ]
	then
		# somethings up, is this a long enough wait?
		echo "Flags and jobs don't add up, waiting and checking again."
		sleep 1m
		if [ $(( numflagsconf + JH_counter )) -ge $totPop ]
		then
			# all good
			echo "Resolved."
		else
			echo "Error occurred."
			echo "Error encountered during AraSim. Waiting 5 minutes for resolution before breaking loop." | mail -s "ERROR" -r "GenetisAlert" parkerkuzma18@gmail.com

			sleep 5m

			if [ $(( numflafsconf + JH_counter )) -ge $totPop ]
			then
				echo "Resolved."

			else
				echo "Error not resolved. Terminating loop."
				echo "Error in AraSim not resolved. Breaking loop. When restarting, begin from either savestate 5 or 6." | mail -s "ERROR" -r "GenetisAlert" parkerkuzma18@gmail.com
				exit 1	
			fi

		fi
	fi





done

rm -f $WorkingDir/Run_Outputs/$RunName/AraSimFlags/*
rm -f $WorkingDir/Run_Outputs/$RunName/AraSimConfirmed/*

wait

cd "$WorkingDir"/Antenna_Performance_Metric


if [ $gen -eq 10000 ]
then
	cp $WorkingDir/Antenna_Performance_Metric/AraOut_ActualBicone.txt $WorkingDir/Run_Outputs/$RunName/AraOut_ActualBicone.txt
fi



#chmod -R 777 /fs/ess/PAS1960/BiconeEvolutionOSC/BiconeEvolution/

