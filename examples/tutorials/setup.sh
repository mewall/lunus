#!/usr/bin/env bash
#
# Dependencies:
# - Conda/miniconda3
#   - To install miniconda, visit https://docs.conda.io/en/latest/miniconda.html
#     and download the appropriate installer package
#       - (note: for Apple Silicon Macs, download the x86 installer,
#         otherwise, you will not have access to gromacs)
#
# - XQuartz, CCP4, coot, and phenix should be installed
#   - To install XQuartz -> https://www.xquartz.org
#   - To install CCP4 + coot -> https://www.ccp4.ac.uk/download
#   - To install phenix -> https://phenix-online.org/download
#
# Set the environment variables below (LUNUS_PATH, PHENIX_PATH, CCP4_PATH and CONDA_PATH) to the absolute paths 
# of the respective directories.
# 
# The lines highlighted below should be placed in your ~/.bash_profile, ~/.bashrc or ~/.zshrc file,
# or placed in a file in this directory (e.g. ./set_env.sh) that can be sourced before running the
# notebook (e.g. source set_env.sh). If placing these command in a ~/.bash_profile, ~/.bashrc or 
# ~/.zshrc file, note that you will need to source that file, or quit the terminal instance and 
# open a new one for the changes to take effect.
# 
# vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
export LUNUS_PATH=${LUNUS_PATH:=/path/to/lunus} # <--- Replace with path to lunus directory cloned from github
export PHENIX_PATH=${PHENIX_PATH:=/path/to/phenix} # <--- Replace with path to phenix install directory
export CCP4_PATH=${CCP4_PATH:=/path/to/ccp4} # <--- Replace with path to ccp4 install directory
source ${PHENIX_PATH}/phenix_env.sh
source ${CCP4_PATH}/bin/ccp4.setup-sh

export CONDA_PATH=${CONDA_PATH:=/path/to/miniconda/} #<--- Replace with path to miniconda directory
source $CONDA_PATH/etc/profile.d/conda.sh
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

# The following code should be run only once.
# If starting from scratch, simply follow the installation instructions at the top of the file,
# and run this whole script with "bash ./setup.sh" 

conda create -n crystalMD python=3.10
conda activate crystalMD
# this one may take a while
conda install -c conda-forge -c bioconda git git-lfs numpy matplotlib cctbx gromacs gnuplot jupyter nb_conda_kernels scons mdtraj parmed nglview ambertools
# ipywidgets version 8 doesn't work with nglview
conda install "ipywidgets <8" -c conda-forge
# need to add these jupyter notebook extensions for nglview
jupyter-nbextension install nglview --py --sys-prefix
jupyter-nbextension enable nglview --py --sys-prefix

# install lunus with scons (not a problem if lunus is already installed)
cd ../../
scons
export PATH="$(pwd)/c/bin/:${PATH}"
cd -
