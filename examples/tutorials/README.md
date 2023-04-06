# Tutorials 

This directory contains tutorials for the preparation and analysis of MD simulations of protein crystals. 

Each directory contains a Jupyter notebook, which is a combination of exposition and code blocks which can be run in line. The directories also contain their own sub-directories containing the files necessary to run the contents of the code blocks in the notebooks. 

- `crystalline_MD_prep/`
    - `MDPreparation.ipynb`: Jupyter notebook walking through the preparation, parametrization, minimization, and equlibration of MD simulation of protein crystals.

- `crystalline_MD_analysis_bragg/`
    - `MDAnalysisBragg.ipynb`: Jupyter notebook walking through the calculation Bragg data from an MD simulation trajectory of a protein crystal, refinement of a protein structure against MD-calculated Bragg data, analysis of B-factors from refinement and simulation, and reverse-propagation of frame of the simulation to produce an ensemble structural model.

- `crystalline_MD_analysis_diffuse/`
    - `MDAnalysisDiffuse.ipynb`: Jupyter notebook walking through teh calculation of diffuse scattering data from an MD simulationtrajectory of protein crystal, calculation of the isotropic scattering (total and by component), and analysis of the covariance of atom pairs from the MD simulation to study models of diffuse scattering.

- `erice-2022`
    - Directory containing a version of the MD prep notebook presented in the Workshop on MD simulations of protein crystals from the 2022 Course on Diffuse Scattering at the Erice International School or Crystallography.

---

To clone this directory, one will need `git` and `git-lfs`. For ease of installation, [Install miniconda3](https://docs.conda.io/en/latest/miniconda.html). Then run `$ conda install -c conda-forge git git-lfs`. Then `$ git clone https://github.com/lanl/lunus.git`. And traverse to this directory with `cd lunus/examples/tutorials/`

These notebooks require a number of dependencies, which can be installed step by step by following the instructions laid out in `INSTALL.md` or by running `bash setup.sh`. 
    - Note that the script `setup.sh` requires some editing before it can be run. `XQuartz`, `CCP4/coot` and `phenix` need to be installed -- the installers can be found at the following links:
        - [XQuartz](https://www.xquartz.org/)
        - [CCP4 + coot](https://www.ccp4.ac.uk/download/)
        - [phenix](https://phenix-online.org/download)
    - Then the path to the directories each of these packages were installed in need to be specified in `setup.sh` (i.e. `export CCP4_PATH="/path/to/ccp4/"` needs to have `/path/to/ccp4` replaced with the absolute path to the directory `ccp4` was installed in on your machine -- e.g. `/User/Applications/ccp4-8.0/`)

---

After following the instructions in `INSTALL.md` or running `bash setup.sh`, one can launch the notebook by changing in to the directory (e.g. `cd crystalline_MD_prep/`) and running `jupyter notebook` followed by the name of the `.ipynb` notebook file (e.g. `jupyter notebook Preparation_of_MD_Analysis_of_Protein_Crystals.ipynb`).
