
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
Seeds=$5
#chmod -R 777 /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/

# Current Plotting Software

module load python/3.7-2019.10

cp AraOut_ActualBicone_10_18.txt Run_Outputs/$RunName/AraOut_ActualBicone.txt

cd Antenna_Performance_Metric
# Format is source directory (where is generationDNA.csv), destination directory (where to put plots), npop
python FScorePlot.py $WorkingDir/Run_Outputs/$RunName $WorkingDir/Run_Outputs/$RunName $NPOP $gen

python3 color_plots.py $WorkingDir/Run_Outputs/$RunName $WorkingDir/Run_Outputs/$RunName $NPOP $gen $Seeds

cd $WorkingDir/Run_Outputs/$RunName

mail -s "FScore_${RunName}_Gen_${gen}" dropbox.2dwp1o@zapiermail.com < FScorePlot2D.png
mail -s "FScore_Color_${RunName}_Gen_${gen}" dropbox.2dwp1o@zapiermail.com < Fitness_Scores_RG.png
mail -s "LRTS_${RunName}_Gen_${gen}" dropbox.2dwp1o@zapiermail.com < LRTSPlot2D.png
mail -s "Veff_${RunName}_Gen_${gen}" dropbox.2dwp1o@zapiermail.com < Veff_plot.png
mail -s "Veff_Color_${RunName}_Gen_${gen}" dropbox.2dwp1o@zapiermail.com < Veffectives_RG.png

cd "$WorkingDir"

echo 'Congrats on getting some nice plots!'

#chmod -R 777 /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/
