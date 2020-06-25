
########    XF Simulation Software (B)     ########################################################################################## 
#
#
#     1. Prepares output.xmacro with generic parameters such as :: 
#             I. Antenna type
#             II. Population number
#             III. Grid size
#
#
#     2. Prepares simulation_PEC.xmacro with information such as:
#             I. Each generation antenna parameters
#
#
#     3. Runs XF and loads XF with both xmacros. 
#
#
###################################################################################################################################### 
# varaibles
indiv=$1
gen=$2
NPOP=$3
## WorkingDir=/users/PAS0654/eliotaferstl/GENETISBicone/BiconeEvolution/current_antenna_evo_build/XF_Loop/Evolutionary_Loop
WorkingDir=$4
RunName=$5
XmacrosDir=$6
## XFProj=$WorkingDir/Run_Outputs/${RunName}/${RunName}.xf
XFProj=$7
GeoFactor=$8
num_keys=$9

# we need to check if directories we're going to write to already exist
# this would occur if already ran this part but went back to rerun the same generation
# the directories are the simulation directories from gen*NPOP+1 to gen*NPOP+10

for i in `seq 1 $NPOP`
do
	# first, declare the number of the individual we are checking
	individual_number=$(($gen*$NPOP + $i))

	# next, write the potential directories corresponding to that individual
	if [ $individual_number -lt 10 ]
	then
		indiv_dir_parent=$XFProj/Simulations/00000$individual_number/
	elif [[ $individual_number -ge 10 && $individual_number -lt 100 ]]
	then
		indiv_dir_parent=$XFProj/Simulations/0000$individual_number/
	elif [[ $individual_number -ge 100 && $individual_number -lt 1000 ]]
	then
		indiv_dir_parent=$XFProj/Simulations/000$individual_number/
	elif [ $individual_number -ge 1000 ]
	then
		indiv_dir_parent=$XFProj/Simulations/00$individual_number/
	fi

	# now delete the directory if it exists
	if [ -d $indiv_dir_parent ]
	then
		rm -R $indiv_dir_parent
	fi

done

# the number of the next simulation directory is held in a hidden file in the Simulations directory
# The file is named .nextSimulationNumber

echo $(($gen*$NPOP + 1)) > $XFProj/Simulations/.nextSimulationNumber



chmod -R 777 $XmacrosDir

#chmod -R 777 /fs/project/PAS0654/BiconeEvolutionOSC/BiconeEvolution/
cd $XmacrosDir
freqlist="8333 10000 11667 13333 15000 16767 18334 20000 21667 23334 25000 26667 28334 30000 31667 33333 35000 36767 38334 40001 41667 43333 45001 46767 48334 50001 51668 53334 55001 56768 58334 60001 61668 63334 65001 66768 68334 70001 71667 73334 75001 76768 78334 80001 81668 83335 85002 86668 88335 90002 91668 93335 95002 96668 98335 100000 101670 103340 105000 106670"
#The list of frequencies, scaled up by 100 to avoid float operation errors in bash
#we have to wait to change the frequencies since we're going to be changing them as we append them to simulation_PEC.xmacro (which is removed below before being remade)

#get rid of the simulation_PEC.xmacro that already exists
rm simulation_PEC.xmacro

#echo "var m = $j;" >> simulation_PEC.xmacro
echo "var NPOP = $NPOP;" > simulation_PEC.xmacro
echo "var indiv = $indiv;" >> simulation_PEC.xmacro
#now we can write the frequencies to simulation_PEC.xmacro
#now let's change our frequencies by the scale factor (and then back down by 100)

#first we need to declare the variable for the frequency lists
#the below commands write the frequency scale factor and "var freq =" to simulation_PEC.xmacro
echo "//Factor of $GeoFactor frequency" >> simulation_PEC.xmacro
echo "var freq " | tr "\n" "=" >> simulation_PEC.xmacro

#here's how we change our frequencies and put them in simulation_PEC.xmacro
for i in $freqlist; #iterating through all values in our list
do
	if [ $i -eq 8333 ] #we need to start with a bracket
	then
		echo " " | tr "\n" "[" >> simulation_PEC.xmacro
		#whenever we append to a file, it adds what we append to a new line at the end
		#the tr command replaces the new line (\n) with a bracket (there's a space at the start; that will separate the = from the list by a space)
	fi

	#now we're ready to start appending our new frequencies
	#we start by changing our frequencies by the scale factor; we'll call this variable k
	k=$(($GeoFactor*$i))
	#now we'll append our frequencies
	#the frequencies we're appending are divided by 100, since the original list was scaled up by 100
	#IT'S IMPORTANT TO DO IT THIS WAY
	#we can't just set k=$((scale*$i/100)) because of how bash handles float operations
	#instead, we need to echo it with the | bc command to allow float quotients
	if [ $i -ne 106670 ] 
	then
		echo "scale=2 ; $k/100 " | bc | tr "\n" "," >> simulation_PEC.xmacro 
		echo "" | tr "\n" " " >> simulation_PEC.xmacro #gives spaces between commas and numbers
	#we have to be careful! we want commas between numbers, but not after our last number
	#hence why we replace \n with , above, but with "]" below
	else 
		echo "scale=2 ; $k/100 " | bc | tr "\n" "]" >> simulation_PEC.xmacro 
		echo " " >> simulation_PEC.xmacro
	fi
	
done

###



###

if [[ $gen -eq 0 && $indiv -eq 1 ]]
then
    echo "if(indiv==1){" >> simulation_PEC.xmacro	
    echo "App.saveCurrentProjectAs(\"$WorkingDir/Run_Outputs/$RunName/$RunName\");" >> simulation_PEC.xmacro
    echo "}" >> simulation_PEC.xmacro
fi

#we cat things into the simulation_PEC.xmacro file, so we can just echo the list to it before catting other files

#cd $XmacrosDir
#cat simulationPECmacroskeleton_GPU.txt >> simulation_PEC.xmacro 

#cat simulationPECmacroskeleton2_GPU.txt >> simulation_PEC.xmacro

cat simulationPECmacroskeleton_GPU_Asym.txt >> simulation_PEC.xmacro
cat simulationPECmacroskeleton2_GPU_Asym.txt >> simulation_PEC.xmacro

#we need to change the gridsize by the same factor as the antenna size
#the gridsize in the macro skeleton is currently set to 0.1
#we want to make it scale in line with our scalefactor

initial_gridsize=0.1
new_gridsize=$(bc <<< "scale=6; $initial_gridsize/$GeoFactor")
sed -i "s/var gridSize = 0.1;/var gridSize = $new_gridsize;/" simulation_PEC.xmacro

sed -i "s+fileDirectory+${WorkingDir}+" simulation_PEC.xmacro
#the above sed command substitute for hardcoded words and don't use a dummy file
#that's ok, since we're doing this after the simulation_PEC.xmacro file has been written; it gets deleted and rewritten from the macroskeletons, so it's ok for us to make changes this way here (as opposed to the way we do it for arasim in parts D1 and D2)

if [[ $gen -ne 0 && $i -eq 1 ]]
then
	cd $XFProj
	rm -rf Simulations
fi

echo
echo
echo 'Opening XF user interface...'
echo '*** Please remember to save the project with the same name as RunName! ***'
echo
echo '1. Import and run simulation_PEC.xmacro'
echo '2. Import and run output.xmacro'
echo '3. Close XF'
#read -p "Press any key to continue... " -n1 -s

module load xfdtd/7.8.1.4

xfdtd $XFProj --execute-macro-script=$XmacrosDir/simulation_PEC.xmacro || true 



## Here is where we need to submit the GPU job
## we want to make this loop over each individual and send each job for fewer minutes
cd $WorkingDir
#qsub -l nodes=1:ppn=40:gpus=1:default -l walltime=0:15:00 -A PAS0654 -v WorkingDir=$WorkingDir,RunName=$RunName,XmacrosDir=$XmacrosDir,XFProj=$XFProj,NPOP=$NPOP,indiv=$indiv GPU_XF_Job.sh 

### End Part B1 ###

### JK! Here's another way of submitting the jobs so that we can give each individual to one job! ###

#
#
#
#
#

#We need to check if the number of keys or the number of individuals is greater
#
if [ $NPOP -lt $num_keys ]
then
	batch_size=$NPOP
else
	batch_size=$num_keys
fi


for m in `seq 1 $batch_size`
do

	######indiv_dir=$XFProj/Simulations/00000$m/Run0001/
	# There's a problem with the above
	# Each time we run XF initially, we make a new simulation folder for each individual
	# Thus, we should have gen*NPOP directories
	# but the above method just overwrites what's held in the first NPOP directories
	# We'll change it to the below:
	
	# first, I want to set a variable for the number of the simulation
	individual_number=$(($gen*$NPOP + $m))
	
	# next, we need to check the length of that number to make the simulation dirctory correctly

	if [ $individual_number -lt 10 ]
	then
		indiv_dir_parent=$XFProj/Simulations/00000$individual_number/
	elif [[ $individual_number -ge 10 && $individual_number -lt 100 ]]
	then
		indiv_dir_parent=$XFProj/Simulations/0000$individual_number/
	elif [[ $individual_number -ge 100 && $individual_number -lt 1000 ]]
	then
		indiv_dir_parent=$XFProj/Simulations/000$individual_number/
	elif [ $individual_number -ge 1000 ]
	then
		indiv_dir_parent=$XFProj/Simulations/00$individual_number/
	fi

	indiv_dir=$indiv_dir_parent/Run0001
	qsub -l nodes=1:ppn=40:gpus=2,mem=178gb -l walltime=0:20:00 -A PAS0654 -v WorkingDir=$WorkingDir,RunName=$RunName,XmacrosDir=$XmacrosDir,XFProj=$XFProj,NPOP=$NPOP,indiv=$individual_number,indiv_dir=$indiv_dir,m=$m GPU_XF_Job.sh ## Here's our job that will do the xfsolver

done

