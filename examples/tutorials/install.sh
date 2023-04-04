#!/bin/bash

# Dependencies:
# ------------
# - Conda/miniconda3
#   - To install miniconda, visit https://docs.conda.io/en/latest/miniconda.html
#     and download the appropriate installer package
#       - (note: for Apple Silicon Macs, download the x86 installer,
#         otherwise, you will not have access to gromacs)
#
# Overview of installation commands:
#   - creates conda environment
#   - installs dependencies
#   - builds lunus

#
# To run this script, if starting from scratch, install conda, edit the line with CONDA_PATH below
# and run `$ bash install.sh` 
#

# -- -- -- -- SET THIS TO THE PATH TO CONDA DIRECTORY -- -- -- --
# vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
export CONDA_PATH=/path/to/conda/installation
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
# -- -- -- -- -- -- -- THANK YOU -- -- -- -- -- -- -- -- -- -- --

source $CONDA_PATH/etc/profile.d/conda.sh

conda create -n crystalMD python=3.10
conda activate crystalMD
# this one may take a while
conda install -c conda-forge -c bioconda git git-lfs numpy matplotlib cctbx gromacs gnuplot jupyter nb_conda_kernels scons mdtraj parmed nglview ambertools
# ipywidgets version 8 doesn't work with nglview
conda install "ipywidgets <8" -c conda-forge
# need to add these jupyter notebook extensions for nglview
jupyter-nbextension install nglview --py --sys-prefix
jupyter-nbextension enable nglview --py --sys-prefix
# tell jupyter to trust the notebooks
jupyter trust crystalline_MD_prep/Preparation_of_MD_Simulations_of_Protein_Crystals.ipynb
jupyter trust crystalline_MD_analysis_bragg/Analysis_of_MD_Simulations_of_Protein_Crystals_Bragg.ipynb
jupyter trust crystalline_MD_analysis_diffuse/Analysis_of_MD_Simulations_of_Protein_Crystals_Diffuse.ipynb
# install lunus with scons (not a problem if lunus is already installed)
cd ../../
scons
cd -


