gen=$1
state=$2
runName=$3
indiv=$4
echo ${gen} > saveStates/$runName.savestate.txt
echo ${state} >> saveStates/$runName.savestate.txt 
echo ${indiv} >> saveStates/$runName.savestate.txt
