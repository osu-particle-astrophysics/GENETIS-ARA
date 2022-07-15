
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
NSECTIONS=$6
#chmod -R 777 /fs/ess/PAS1960/BiconeEvolutionOSC/BiconeEvolution/

# Current Plotting Software

cd $WorkingDir

module load python/3.7-2019.10

#cp AraOut_ActualBicone_10_18.txt Run_Outputs/$RunName/AraOut_ActualBicone.txt
#cp ARA_Bicone_Data/AraOut_Actual_Bicone_Fixed_Polarity_2.9M_NNU.txt Run_Outputs/$RunName/AraOut_ActualBicone.txt

## Take the weighted average of the fitness score of repeated individuals
#python Data_Generators/fitness_check.py ${gen} Generation_Data/

cd Antenna_Performance_Metric
# Format is source directory (where is generationDNA.csv), destination directory (where to put plots), npop
python FScorePlot.py $WorkingDir/Run_Outputs/$RunName $WorkingDir/Run_Outputs/$RunName $NPOP $gen

python color_plots.py $WorkingDir/Run_Outputs/$RunName $WorkingDir/Run_Outputs/$RunName $NPOP $gen $Seeds

mkdir -m 775 $WorkingDir/Run_Outputs/$RunName/Gain_Plots/${gen}_Gain_Plots
./image_maker.sh $WorkingDir/Run_Outputs/$RunName/ $WorkingDir/../Xmacros/ $WorkingDir/Run_Outputs/$RunName/Antenna_Images/ $gen $WorkingDir $RunName $NPOP

cd $WorkingDir/Run_Outputs/$RunName
#mail -s "FScore_${RunName}_Gen_${gen}" dropbox.2dwp1o@zapiermail.com < FScorePlot2D.png
#mail -s "FScore_Color_${RunName}_Gen_${gen}" dropbox.1dwp1o@zapiermail.com < Fitness_Scores_RG.png
#mail -s "Veff_${RunName}_Gen_${gen}" dropbox.2dwp1o@zapiermail.com < Veff_plot.png
#mail -s "Veff_Color_${RunName}_Gen_${gen}" dropbox.2dwp1o@zapiermail.com < Veffectives_RG.png
#mail -s "LRAB_${RunName}_Gen_${gen}" dropbox.2dwp1o@zapiermail.com < LRABPlot2D.png
#mail -s ${gen}_Gain_Plots/polar_plot_300.png
cd "$WorkingDir"

echo 'Congrats on getting some nice plots!'

## I'm going to get rid of all of the slurm files being created

rm -f slurm-*

