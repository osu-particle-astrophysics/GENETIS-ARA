#!/bin/bash

# this script is a test script for moving uan files to different names based on the individual
# number in the generation (population), rather than in total

for i in `seq 11 20`
do
	pop_ind_num=$(($i - 10))
	for freq in `seq 1 60`
	do
		mv 1_${i}_${freq}.uan 1_${pop_ind_num}_${freq}.uan
	done
done
