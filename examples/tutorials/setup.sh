#!/usr/bin/env bash
#
# DEPENDENCIES:
# -------------
# - XQuartz, CCP4, coot, and phenix should be installed
#   - To install XQuartz -> https://www.xquartz.org
#   - To install CCP4 + coot -> https://www.ccp4.ac.uk/download
#   - To install phenix -> https://phenix-online.org/download
#
# TODO UPON FIRST USE:
# -------------------
#   - Edit and run install.sh (`$ bash install.sh`)
#   - Set the environment variables below (LUNUS_PATH, PHENIX_PATH, CCP4_PATH and CONDA_PATH) to the absolute paths 
#     of the respective directories.
# 
# If starting from scratch, set the environment variables below by editing this file
# to set the right paths to lunus, phenix, and ccp4
#
# Then, before running the notebooks, source the environment variables in this script with `$ source ./setup.sh`

# -- -- -- -- -- -- -- -- -- SET THESE ENVIRONMENT VARIABLES FOR YOUR SYSTEM -- -- -- -- -- -- -- -- -- -- -- -- --
# vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
export LUNUS_PATH=${LUNUS_PATH:=/path/to/lunus}      # <--- Replace with path to lunus directory cloned from github
export PHENIX_PATH=${PHENIX_PATH:=/path/to/phenix}   # <--- Replace with path to phenix install directory
export CCP4_PATH=${CCP4_PATH:=/path/to/ccp4}         # <--- Replace with path to ccp4 install directory
export CONDA_PATH=${CONDA_PATH:=/path/to/miniconda/} #<--- Replace with path to miniconda directory
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
# -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- THANK YOU -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

export PATH=${LUNUS_PATH}/c/bin/:$PATH
source ${PHENIX_PATH}/phenix_env.sh
source ${CCP4_PATH}/bin/ccp4.setup-sh
source ${CONDA_PATH}/etc/profile.d/conda.sh

