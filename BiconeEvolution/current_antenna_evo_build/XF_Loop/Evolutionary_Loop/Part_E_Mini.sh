!/bin/bash
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
gen=0
NPOP=2
WorkingDir=`pwd`
RunName=shorty_6_4
ScaleFactor=1
Seeds=10
indiv=0
GeoFactor=1.0

#chmod -R 777 /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/

module load python/3.7-2019.10

cd Antenna_Performance_Metric

echo 'Starting fitness function calculating portion...'

mv *.root "$WorkingDir/Run_Outputs/$RunName/RootFilesGen${gen}/"

#Check what this first line does. It can probably be taken out.
#InputFiles="FitnessFunction.exe $NPOP"
for i in `seq $indiv $NPOP`
do
  	InputFiles="${InputFiles}AraOut_${gen}_${i}.txt " #had .txt but that's also in the fitnessfunction executable
       
done

./fitnessFunction.exe $NPOP $Seeds $ScaleFactor $WorkingDir/generationDNA.csv $GeoFactor $InputFiles #Here's where we add the flags for the generation
cp fitnessScores.csv "$WorkingDir"/Run_Outputs/$RunName/${gen}_fitnessScores.csv
mv fitnessScores.csv "$WorkingDir"

cp vEffectives.csv "$WorkingDir"/Run_Outputs/$RunName/${gen}_vEffectives.csv
mv vEffectives.csv "$WorkingDir"

cp errorBars.csv "$WorkingDir"/Run_Outputs/$RunName/${gen}_errorBars.csv
mv errorBars.csv "$WorkingDir"

#Plotting software for Veff(for each individual) vs Generation
python Veff_Plotting.py "$WorkingDir"/Run_Outputs/$RunName "$WorkingDir"/Run_Outputs/$RunName $gen $NPOP $Seeds

# Note: gensData.py floats around in the main dir until it is moved to 
# Antenna_Performance_Metric

echo 'Congrats on getting a fitness score!'

#chmod -R 777 /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/
