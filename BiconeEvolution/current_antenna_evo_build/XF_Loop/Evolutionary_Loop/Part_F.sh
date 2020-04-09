
########  Plotting (F)  ############################################################################################################################ 
#
#
#      1. Plots in 3D and 2D of current and all previous generation's scores. Saves the 2D plots. Extracts data from $RunName folder in all of the i_generationDNA.csv files. Plots to same directory.
#
#
#################################################################################################################################################### 
# variables
NPOP=$1
WorkingDir=$2
RunName=$3
gen=$4
#chmod -R 777 /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/

# Current Plotting Software

cd Antenna_Performance_Metric
# Format is source directory (where is generationDNA.csv), destination directory (where to put plots), npop
python FScorePlot.py $WorkingDir/Run_Outputs/$RunName $WorkingDir/Run_Outputs/$RunName $NPOP $gen

#

cd $WorkingDir/Run_Outputs/$RunName
mail -s "${RunName}_Gen_${gen}" dropbox.2dwp1o@zapiermail.com < FScorePlot2D.png

cd "$WorkingDir"

echo 'Congrats on getting some nice plots!'

#chmod -R 777 /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/
