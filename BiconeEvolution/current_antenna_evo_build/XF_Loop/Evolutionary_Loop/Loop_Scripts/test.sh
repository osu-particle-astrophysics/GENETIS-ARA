RunName='Rolla_halfscalefactor'           ## Replace when needed                                                                 
TotalGens=1                     ## number of generations (after initial) to run through                                
WorkingDir=`pwd` 
NPOP=2                          ## number of individuals per generation; please keep this value below 99                
FREQ=1                         ## frequencies being iterated over in XF (Currectly only affects the output.xmacro loop\
                                                                                                                       
NNT=10000                          ##Number of Neutrinos Thrown in AraSim                                               
ScaleFactor=1.0                   ##ScaleFactor used when punishing fitness scores of antennae larger than holes used i\

AntennaRadii=/fs/ess/PAS1960/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build/XF_Loop/Evolutionary_Loop/Run_Outputs/${RunName}



#cd "$WorkingDir"/Antenna_Performance_Metric
       
        
 #       for i in `seq 1 $NPOP`
  #      do
   #         InputFiles="${InputFiles}AraOut_${i}.txt "
    #    done


./fitnessFunction_Araseed.exe $NPOP $ScaleFactor $WorkingDir/4_generationDNA.csv $InputFiles
#cd "$WorkingDir"
# rm runData.csv
#python gensData.py 1
cd Antenna_Performance_Metric
#python LRPlot.py "$WorkingDir" "$WorkingDir"/Run_Outputs/$RunName 1 $NPOP
python LRTPlot.py "$WorkingDir"/Antenna_Performance_Metric "$WorkingDir"/Run_Outputs/$RunName $TotalGens $NPOP  
#python FScorePlot.py "$WorkingDir"/Run_Outputs/$RunName "$WorkingDir"/Run_Outputs/$RunName $TotalGens
#python Veff_Plotting.py "$WorkingDir"/Run_Outputs/$RunName "$WorkingDir"/Run_Outputs/$RunName $TotalGens $NPOP    
#python leogaintest.py $NPOP
#python gain_match_2.py $NPOP
#gen=2
#indiv=1
#python XFintoARA.py $NPOP $WorkingDir $RunName $gen $indiv
#python XFintoARA.py $NPOP
