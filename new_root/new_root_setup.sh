#!/bin/sh
# env to use the ROOT and boost from ARA cvmfs 

# this is important, the ARA cvmfs needs GCC ~ 4.8.5
module purge
module load gnu/4.8.5

export ARA_SETUP_DIR="/cvmfs/ara.opensciencegrid.org/trunk/centos7"
export ARA_DEPS_INSTALL_DIR="${ARA_SETUP_DIR%/}/misc_build"
export LD_LIBRARY_PATH="$ARA_DEPS_INSTALL_DIR/lib:$LD_LIBRARY_PATH"
export PATH="$ARA_UTIL_INSTALL_DIR/bin:$ARA_DEPS_INSTALL_DIR/bin:$PATH"

. "${ARA_SETUP_DIR%/}/root_build/bin/thisroot.sh"

export BOOST_ROOT="$ARA_DEPS_INSTALL_DIR/include"
