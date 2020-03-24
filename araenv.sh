
#Laurens ROOT installation. Use for BinnedAnalysis or anitaBuildTools.                                                                                                                               
source /users/PAS0654/osu9979/root/bin/thisroot.sh #Laurens root version 

module load python/3.6-conda5.2

export ANITA_UTIL_INSTALL_DIR=/fs/project/PAS0654/BiconeEvolutionOSC/anitaBuildTool/utils
export ANITA_UTIL_INC_DIR=${ANITA_UTIL_INSTALL_DIR}/include
export LD_LIBRARY_PATH=${ANITA_UTIL_INSTALL_DIR}/lib:${LD_LIBRARY_PATH}
export PATH=${ANITA_UTIL_INSTALL_DIR}/bin:${PATH}

export FFTWDIR=$FFTW3_HOME
export PATH=$FFTWDIR/bin:$PATH
export LD_LIBRARY_PATH=$FFTWDIR/lib:$LD_LIBRARY_PATH
export LD_INCLUDE_PATH=$FFTWDIR/include:$LD_INCLUDE_PATH
export LD_INCLUDE_PATH=/fs/project/PAS0654/cint/libcint/build/include:$LD_INCLUDE_PATH
export CPLUS_INCLUDE_PATH=/fs/project/PAS0654/BiconeEvolutionOSC/cint/libcint/build/include:$CPLUS_INCLUDE_PATH:$ANITA_UTIL_INSTALL_DIR/include:~/app/include
 
alias l="ls"
alias root="root -l"

#Set up some cint stuff (sam's code needs it)
export CINTSYSDIR=/fs/project/PAS0654/cint
export PATH=$CINTSYSDIR:$PATH
export MANPATH=$CINTSYSDIR/doc:$MANPATH
export LD_LIBRARY_PATH=$CINTSYSDIR:.:$LD_LIBRARY_PATH
export LD_ELF_LIBRARY_PATH=$LD_LIBRARY_PATH
 
##########
# modifications by HEALPixAutoConf 3.31
[ -r /users/PAS0174/osu8620/.healpix/3_31_Linux/config ] && . /users/PAS0174/osu8620/.healpix/3_31_Linux/config
 
PS1='${debian_chroot:+($debian_chroot)}oindree:\u\w\$ '
