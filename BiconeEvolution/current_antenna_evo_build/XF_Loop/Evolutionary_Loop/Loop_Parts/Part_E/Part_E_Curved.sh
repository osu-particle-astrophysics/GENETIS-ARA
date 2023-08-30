
########  Fitness Score Generation (E)  ######################################################################################################### 
#
#
#      1. Takes AraSim data and cocatenates each file name into one string that is then used to generate fitness scores 
#
#      2. Then gensData.py extracts useful information from generationDNA.csv and fitnessScores.csv, and writes to maxFitnessScores.csv and runData.csv
#
#      3. Copies each .uan file from the Antenna_Performance_Metric folder and moves to Run_Outputs/$RunName folder
#
#
#################################################################################################################################################### 

#variables
gen=$1
NPOP=$2
WorkingDir=$3
RunName=$4
ScaleFactor=$5
indiv=$6
Seeds=$7
GeoFactor=$8
AraSimExec=$9
XFProj=${10}
NSECTIONS=${11}
SEPARATION=${12}
CURVED=${13}
ParallelAra=${14}
#chmod -R 777 /fs/ess/PAS1960/BiconeEvolutionOSC/BiconeEvolution/

if [ $ParallelAra -eq 1 ]
then
	ara_processes=$((Seeds*10))
else
	ara_processes=$Seeds
fi

module load python/3.7-2019.10

cd $WorkingDir
# put the actual bicone results in the run name directory
cp ARA_Bicone_Data/AraOut_Actual_Bicone_Fixed_Polarity_2.9M_NNU.txt Run_Outputs/$RunName/Generation_Data/Generation_${gen}/AraOut_ActualBicone.txt

cd Antenna_Performance_Metric/

echo 'Starting fitness function calculating portion...'
mkdir -m775 $WorkingDir/Run_Outputs/$RunName/Root_Files/Root_Files_${gen}
mv *.root $WorkingDir/Run_Outputs/$RunName/Root_Files/Root_Files_${gen}/

for i in `seq $indiv $NPOP`
do
  	InputFiles="${InputFiles}AraOut_${gen}_${i}.txt " #had .txt but that's also in the fitnessfunction executable
       
done

# we'll use a different fitnessFunction executable for the symmetric/asymmetric cases
# for now I'm just using different executables, but once we're confident on the cpp files
# we can just recompile to the same executable
if [ $NSECTIONS -eq 1 ]
then
	## In the case where NSECTIONS is 1, we have a symmetric antenna
	## In this case, let's compile the regular, symmetric fitness function
	g++ -std=c++11 fitnessFunction_ARA.cpp -o fitnessFunction.exe

	## Since it requires different arguments from other versions, run it in this if statement
	./fitnessFunction.exe $NPOP $ara_processes $ScaleFactor $WorkingDir/Generation_Data/generationDNA.csv $GeoFactor $InputFiles #Here's where we add the flags for the generation

else
	## In this case, we can evolve the separation distance or not
	## So we'll need another if-else statement
	if [ $SEPARATION -eq 1 ] ## Evolving the separation
	then
		## compile the separation distance fitness function
		g++ -std=c++11 fitnessFunction_ARA_Sep.cpp -o fitnessFunction_Sep.exe

		## Now run the executable
		./fitnessFunction_Sep.exe $NPOP $ara_processes $ScaleFactor $WorkingDir/Generation_Data/generationDNA.csv $GeoFactor $InputFiles #Here's where we add the flags for the generation

	else
		if [ $CURVED -eq 0 ]
		then
			g++ -std=c++11 fitnessFunction_ARA_Asym.cpp -o fitnessFunction_asym.exe
	
			## Now run the newly compiled executable
			./fitnessFunction_asym.exe $NPOP $ara_processes $ScaleFactor $WorkingDir/Generation_Data/generationDNA.csv $GeoFactor $InputFiles #Here's where we add the flags for the generation
		else
			g++ -std=c++11 fitnessFunction_ARA_curved.cpp -o fitnessFunction_curved.exe
			./fitnessFunction_curved.exe $NPOP $ara_processes $ScaleFactor $WorkingDir/Generation_Data/generationDNA.csv $GeoFactor $InputFiles #Here's where we add the flags for the generation
		fi
	fi

fi

echo "Finished fitness function"

cp fitnessScores.csv $WorkingDir/Run_Outputs/$RunName/Generation_Data/Generation_${gen}/${gen}_fitnessScores.csv
mv fitnessScores.csv $WorkingDir/Generation_Data/

cp vEffectives.csv $WorkingDir/Run_Outputs/$RunName/Generation_Data/Generation_${gen}/${gen}_vEffectives.csv
mv vEffectives.csv $WorkingDir/Generation_Data/

cp errorBars.csv $WorkingDir/Run_Outputs/$RunName/Generation_Data/Generation_${gen}/${gen}_errorBars.csv
mv errorBars.csv $WorkingDir/Generation_Data/

# Let's produce the plot of the gain pattern for each of the antennas
# Start by making a directory to contain the images for the gain patterns of that generation
### NOTE: Moved to Part F in image_maker.sh
#mkdir -m 775 $WorkingDir/Run_Outputs/$RunName/${gen}_Gain_Plots
#python $WorkingDir/Antenna_Performance_Metric/polar_plotter.py $WorkingDir/Run_Outputs/$RunName $WorkingDir/Run_Outputs/$RunName/${gen}_Gain_Plots 14 $NPOP $gen

#Plotting software for Veff(for each individual) vs Generation
python Veff_Plotting.py $WorkingDir/Run_Outputs/$RunName $WorkingDir/Run_Outputs/$RunName/Evolution_Plots $gen $NPOP $Seeds 

cd $WorkingDir

if [ $gen -eq 0 ]
then
	rm -f Generation_Data/runData.csv
fi

if [ $indiv -eq $NPOP ]
then
	cp Generation_Data/runData.csv $WorkingDir/Run_Outputs/$RunName/runData_$gen.csv
fi


python Data_Generators/gensData_asym.py $gen $NSECTIONS $NPOP Generation_Data

#This is where we'll make the rainbow plot
python Antenna_Performance_Metric/DataConverter_quad.py
/cvmfs/ara.opensciencegrid.org/trunk/centos7/misc_build/bin/python3.9 Antenna_Performance_Metric/Rainbow_Plotter.py
## Jacob call Dennis' script here!
## Needs the path to the root files as an argument
mv Generation_Data/Rainbow_Plot.png Run_Outputs/$RunName/Evolution_Plots/Rainbow_Plot.png


cd Antenna_Performance_Metric
next_gen=$((gen+1))

# Since we know we're using the curved on, just call the curved LRAB plot script
python LRABPlot.py $WorkingDir/Generation_Data $WorkingDir/Run_Outputs/$RunName $next_gen $NPOP $GeoFactor $NSECTIONS

# Run 3D Plots of L,R,T vs Fitness
#python 3DLength.py "$WorkingDir" "$WorkingDir"/Run_Outputs/$RunName $next_gen $NPOP $GeoFactor $NSECTIONS
#python 3DRadius.py "$WorkingDir" "$WorkingDir"/Run_Outputs/$RunName $next_gen $NPOP $GeoFactor $NSECTIONS
#python 3DTheta.py "$WorkingDir" "$WorkingDir"/Run_Outputs/$RunName $next_gen $NPOP $GeoFactor $NSECTIONS

cd ..


#we want to record the gain data each time
cd $AraSimExec
for i in `seq 1 $NPOP`
do
	mv a_${i}.txt $XFProj/XF_model_${gen}_${i}.txt
done

cd $WorkingDir/Antenna_Performance_Metric

#python3 avg_freq.py $XFProj $gen $XFProj 10 $NPOP

cd $XFProj
#mv gain_vs_freq.png gain_vs_freq_gen_${gen}.png
#mv phase_vs_freq.png phase_vs_freq_${gen}.png

echo 'Congrats on getting a fitness score!'

cd $WorkingDir/Run_Outputs/$RunName

mkdir -m777 AraOut/AraOut_${gen}
cd $WorkingDir/Antenna_Performance_Metric
for i in `seq 1 $NPOP`
do
	for j in `seq 1 $Seeds`
	do
	mv AraOut_${gen}_${i}_${j}.txt $WorkingDir/Run_Outputs/$RunName/AraOut/AraOut_${gen}/AraOut_${gen}_${i}_${j}.txt
	done

done 
