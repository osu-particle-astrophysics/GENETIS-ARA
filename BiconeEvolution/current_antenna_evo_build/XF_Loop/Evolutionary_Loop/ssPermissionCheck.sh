#!/bin/bash
WorkingDir="/users/PAS0654/kuzma31/Mail_Notif"
runname=$1  # current loop run name
savestatefile="/fs/ess/PAS1960/BiconeEvolutionOSC/BiconeEvolution/current_antenna_evo_build/XF_Loop/Evolutionary_Loop/saveStates/${runname}.savestate.txt" # savestate file for current run
sserrormess='Savestate did not update after jobs finished. Permissions were likely not opened after the last run.' # link possible?
statevar=$2 # variable of current loop state

line=1
while IFS= read -r val
do
        if [ $line -eq 2 ]
        then
                statefromfile=$val
                if [ $statefromfile -eq $statevar ]
                then
                        # all is well
                        echo "Savestate is in the correct state."
                else
                        # something has gone wrong, wait a bit
                        echo "Savestate variable doesn't match file, waiting and trying again."
                        sleep 1m
                        # check again
                        if [ $statefromfile -eq $statevar ]
                        then
                                # all is well
                                echo "Resolved."
                        else
                                # bad
                                echo "Error. Sending email."
                                mail -s "ERROR" -r "GenetisAlert" parkerkuzma18@gmail.com <<< $sserrormess
                        fi
                fi
        break
        fi
        line=$((line+1))
done < $savestatefile	
