#The purpose of this script is to open permission on all the directories that are important to chmod after a run

#It should be run with 1 variable, the run name

WorkingDir=/fs/ess/PAS1960/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build/XF_Loop/Evolutionary_Loop

WorkingDir=`pwd`
AraSimExec="${WorkingDir}/../../../../AraSim"
XmacrosDir=$WorkingDir/../Xmacros
RunName=$1


chmod 777 *
cd $WorkingDir/saveStates
chmod 777 *
cd $WorkingDir/Antenna_Performance_Metric
chmod 777 *
cd $XmacrosDir
chmod 777 *
cd $AraSimExec/..
chmod -R 777 AraSim
cd $WorkingDir/Run_Outputs/
chmod -R 777 $RunName
