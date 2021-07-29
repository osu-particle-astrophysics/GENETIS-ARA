
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

#chmod -R 777 /fs/ess/PAS1960/BiconeEvolutionOSC/BiconeEvolution/

module load python/3.7-2019.10

cd $WorkingDir
# put the actual bicone results in the run name directory
cp ARA_Bicone_Data/AraOut_Actual_Bicone_Fixed_Polarity_2.9M_NNU.txt Run_Outputs/$RunName/AraOut_ActualBicone.txt

cd Antenna_Performance_Metric/

echo 'Starting fitness function calculating portion...'

mv *.root "$WorkingDir/Run_Outputs/$RunName/RootFilesGen${gen}/"

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
	./fitnessFunction.exe $NPOP $Seeds $ScaleFactor $WorkingDir/Generation_Data/generationDNA.csv $GeoFactor $InputFiles #Here's where we add the flags for the generation

else
	## In this case, we can evolve the separation distance or not
	## So we'll need another if-else statement
	if [ $SEPARATION -eq 1 ] ## Evolving the separation
	then
		## compile the separation distance fitness function
		g++ -std=c++11 fitnessFunction_ARA_Sep.cpp -o fitnessFunction_Sep.exe

		## Now run the executable
		./fitnessFunction_Sep.exe $NPOP $Seeds $ScaleFactor $WorkingDir/Generation_Data/generationDNA.csv $GeoFactor $InputFiles #Here's where we add the flags for the generation

	else
		g++ -std=c++11 fitnessFunction_ARA_Asym.cpp -o fitnessFunction_asym.exe

		## Now run the newly compiled executable
		./fitnessFunction_asym.exe $NPOP $Seeds $ScaleFactor $WorkingDir/Generation_Data/generationDNA.csv $GeoFactor $InputFiles #Here's where we add the flags for the generation
	fi

fi

cp fitnessScores.csv $WorkingDir/Run_Outputs/$RunName/${gen}_fitnessScores.csv
mv fitnessScores.csv $WorkingDir/Generation_Data/

cp vEffectives.csv $WorkingDir/Run_Outputs/$RunName/${gen}_vEffectives.csv
mv vEffectives.csv $WorkingDir/Generation_Data/

cp errorBars.csv $WorkingDir/Run_Outputs/$RunName/${gen}_errorBars.csv
mv errorBars.csv $WorkingDir/Generation_Data/

#Plotting software for Veff(for each individual) vs Generation
python Veff_Plotting.py $WorkingDir/Run_Outputs/$RunName $WorkingDir/Run_Outputs/$RunName $gen $NPOP $Seeds 

cd $WorkingDir

if [ $gen -eq 0 ]
then
	rm Generation_Data/runData.csv
fi

if [ $indiv -eq $NPOP ]
then
	cp Generation_Data/runData.csv $WorkingDir/Run_Outputs/$RunName/runData_$gen.csv
fi

python Data_Generators/gensData_asym.py $gen $NSECTIONS $NPOP Generation_Data
cd Antenna_Performance_Metric
next_gen=$((gen+1))

# I can potentially simplify this further by making the if statement in LRTPlot instead
if [ $NSECTIONS -eq 1 ]
then
	python LRTPlot.py $WorkingDir/Generation_Data $WorkingDir/Run_Outputs/$RunName $next_gen $NPOP $GeoFactor
else
	## Let's consider whether or not we're evolving the separation distance
	if [ $SEPARATION -eq 0 ]
	then
		python LRTPlot2.0.py $WorkingDir/Generation_Data $WorkingDir/Run_Outputs/$RunName $next_gen $NPOP $GeoFactor $NSECTIONS	
	else
		python LRTSPlot.py $WorkingDir/Generation_Data $WorkingDir/Run_Outputs/$RunName $next_gen $NPOP $GeoFactor $NSECTIONS
	fi
fi

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

mkdir -m777 AraOut_$gen
cd $WorkingDir/Antenna_Performance_Metric
for i in `seq 1 $NPOP`
do
    for j in `seq 1 $Seeds`
    do

	cp AraOut_${gen}_${i}_${j}.txt $WorkingDir/Run_Outputs/$RunName/AraOut_${gen}/AraOut_${gen}_${i}_${j}.txt
	
	done

done 

cd $WorkingDir

# I still need to add these changes into the asymmetric algorithm
if [ $gen -gt 0 ]
then
	mv Generation_Data/parents.csv Run_Outputs/$RunName/${gen}_parents.csv
	mv Generation_Data/genes.csv Run_Outputs/$RunName/${gen}_genes.csv
	mv Generation_Data/mutations.csv Run_Outputs/$RunName/${gen}_mutations.csv
fi

mv Generation_Data/generators.csv Run_Outputs/$RunName/${gen}_generators.csv

#chmod -R 777 /fs/ess/PAS1960/BiconeEvolutionOSC/BiconeEvolution/
