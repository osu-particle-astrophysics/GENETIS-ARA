#This is a version of Part_D2 that uses multiple Seeds for an individual run of AraSim

#variables
gen=$1
NPOP=$2
WorkingDir=$3
RunName=$4
Seeds=$5
AraSimExec=$6

#chmod -R 777 /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/

cd $WorkingDir/Run_Outputs/$RunName/AraSimFlags/
#cd $WorkingDir/Run_Outputs/AraSimFlags/
nFiles=0

totPop=$( expr $NPOP \* $Seeds )

while [ "$nFiles" != "$totPop" ]
do
	echo "Waiting for AraSim jobs to finish..."
	sleep 60
	# we need to base the counter off of the new flags
	# these are in the AraSimConfirmed directory
	nFiles=$(ls -1 --file-type ../AraSimConfirmed | grep -v '/$' | wc -l) 
	
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
				rm ../${gen}_AraSim_Errors/${current_file}.error
				rm ../${gen}_AraSim_Outputs/${current_file}.output
				
				# now we can resubmit the job
				cd $WorkingDir

				qsub -v num=${current_individual},WorkingDir=$WorkingDir,RunName=$RunName,Seeds=${current_seed},AraSimDir=$AraSimExec,gen=$gen AraSimCall_AraSeed.sh
				
				cd Run_Outputs/$RunName/AraSimFlags/

				# since we need to rerun, we need to remove the flag
				rm ${current_individual}_${current_seed}.txt

			else
				# we need to add the second flag to denote that all is well if there was not error
				echo "This individual succeeded" > ../AraSimConfirmed/${current_individual}_${current_seed}_confirmation.txt
			fi
		fi
	done
done

rm -f $WorkingDir/Run_Outputs/$RunName/AraSimFlags/*
rm -f $WorkingDir/Run_Outputs/$RunName/AraSimConfirmed/*

wait

cd "$WorkingDir"/Antenna_Performance_Metric


if [ $gen -eq 100 ]
then
	cp $WorkingDir/Antenna_Performance_Metric/AraOut_ActualBicone.txt $WorkingDir/Run_Outputs/$RunName/AraOut_ActualBicone.txt
fi



#chmod -R 777 /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/

