## Dependencies:
- `conda` installable (see below):
    - [Jupyter](https://jupyter.org)
    - [MDTraj](https://www.mdtraj.org/)
    - [cctbx](https://github.com/cctbx/cctbx_project)
    - [NumPy](https://numpy.org)
    - [matplotlib](https://matplotlib.org)
    - [GROMACS](https://www.gromacs.org)
    - [ambertools](https://ambermd.org/AmberTools.php)
    - [gnuplot](https://gnuplot.sourceforge.net)
    - [SCons](https://scons.org)
    - [git](https://git-scm.com)
    - [git-lfs](https://git-lfs.com)
    - [parmed](https://parmed.github.io/ParmEd/html/index.html)
    - [nglview](https://github.com/nglviewer/nglview)
- Installable with a GUI/package manager (see below):
    - [XQuartz](https://www.xquartz.org/)
    - [CCP4 + coot](https://www.ccp4.ac.uk/download/)
    - [phenix](https://phenix-online.org/download)
- Installed from the command-line using `git` (see below):
    - [lunus](https://github.com/lanl/lunus)


## Setup (from scratch):

_Below, any command with a `$` symbol in front of it means that that command should be run from the terminal/command line. Otherwise, the commands should be placed in your `~/.zshrc`, `~/.bash_profile`, `~/.bashrc` or similar, or placed in a script to be sourced before running the notebooks (e.g. `source set_env.sh`)._

**After installing `conda/miniconda`, `XQuartz`, `CCP4`, `coot`, and `phenix`, all of the following can be accomplished by editing and running the script `setup.py` in this directory (e.g. `$ bash setup.py`). However, for explanations of all the commands, or to run the installations line-by-line/instruction-by-instruction from the terminal yourself (e.g. if you encounter problems with the install) follow the instructions below.**

**Note:** We assume a bash-compatible shell for all of the following setup commands and the running of the notebooks themselves. These instructions will have to be altered to be compatible with other shells.

---

### Installing Conda and Conda-installable packages and creating a conda environment
- [Install `miniconda3`](https://docs.conda.io/en/latest/miniconda.html)
    - _note: if you're using an ARM (M1/M2) Mac, you will nonetheless need to install the x86 version of miniconda3, not the ARM version, to have access to `GROMACS`. `GROMACS` is not (currently, as of the publishing of this notebook) installable on ARM Macs through the ARM version `conda`. We have not noticed any issues with the x86 miniconda3 installation on our own ARM Macs, besides maybe some diminished speed._

- source the conda setup script

```export CONDA_PATH=/path/to/miniconda/```

- replace the path above with the path to the miniconda installation directory.

```source $CONDA_PATH/etc/profile.d/conda.sh```


- Create a new conda envorinment (in this case, named "`crystalMD`", but you can name it whatever you like with the argument after the `-n` flag): 

```$ conda create -n crystalMD python=3.10```

- Activate that environment: 

```$ conda activate crystalMD```

- Install the conda-installable dependencies:

```$ conda install -c conda-forge -c bioconda git git-lfs numpy matplotlib cctbx gromacs gnuplot jupyter nb_conda_kernels scons mdtraj parmed nglview ambertools```

- Configure `ipywidgets` and `nglview` so that the `jupyter` notebooks will run correctly:

```$ conda install "ipywidgets <8" -c conda-forge```
```$ jupyter-nbextension install nglview --py --sys-prefix```
```$ jupyter-nbextension enable nglview --py --sys-prefix```

- Tell jupyter to trust the notebooks before opening them:

```$ jupyter trust crystalline_MD_prep/Preparation_of_MD_Simulations_of_Protein_Crystals.ipynb```
```$ jupyter trust crystalline_MD_analysis_bragg/Analysis_of_MD_Simulations_of_Protein_Crystals_Bragg.ipynb```
```$ jupyter trust crystalline_MD_analysis_diffuse/Analysis_of_MD_Simulations_of_Protein_Crystals_Diffuse.ipynb``` 

- To open the notebooks:

```$ jupyter notebook [name of the notebook file].ipynb```

or, simply

```$ jupyter notebook```

from the directory where the notebook lives, and double click on the notebook to open it.

- To have access to all of these installed packages while running the `jupyter` notebooks, select the `Python [conda env:crystalMD]` kernel in the `Kernel > Change Kernel` dropdown menu of the notebook.

---

### Installing CCP4 and Phenix

- [Install `XQuartz`](https://www.xquartz.org/) before installing [`CCP4 + coot`](https://www.ccp4.ac.uk/download/) and [`phenix`](https://phenix-online.org/download)
    - The `CCP4 + coot` link above will take you to a page with a package manager that can be used to install the `CCP4` program suite (`SHELX` and `ARP` are not required, the simplest package manager is fine.)
    - The `phenix` link above will take you to a page where you can request a password to download the installer -- `phenix` is free for academic users -- and with the provided username and password, you will be able to download an installer. 


Make sure to record what directory you installed `CCP4` and `phenix` into, and the directory where lunus was cloned from github, then set the following environment variables in your `~/.bashrc`, `~/.bash_profile`, `~/.zshrc`, file or the equivalent for your system/shell:

```export LUNUS_PATH="/path/to/your/lunus/directory/"```

```export PHENIX_PATH="/path/to/your/phenix/directory/"```

```export CCP4_PATH="/path/to/your/ccp4/directory/"```

- Once `phenix` has been installed, put the `phenix.refine` and other `phenix` commands on path by sourcing the environment script (add the below to your `~/.bashrc`, `~/.bash_profile`, `~/.zshrc`, file or the equivalent for your system/shell):

```source ${PHENIX_PATH}/phenix_env.sh```

- Add `ccp4` commands to the command_line by sourcing the setup script (add the below to your `~/.bashrc`, `~/.bash_profile`, `~/.zshrc`, file or the equivalent for your system/shell):

```source ${CCP4_PATH}/bin/ccp4.setup-sh```

After all of these commands have been added to your `~/.bashrc`, `~/.bash_profile`, `~/.zshrc`, file or the equivalent for your system/shell, you'll have to either `source` that file, or quit and restart your terminal for the changes to go into effect. You can also place the above commands in a script in this directory (e.g. `set_env.sh`) and source it before running the notebooks.
        
---

### Installing lunus:

- Change in to the root directory:

```$ cd ../../```

- Install lunus methods:

```$ scons```

- add the lunus commands to the path, by adding the below to your `~/.bashrc`, `~/.bash_profile`, `~/.zshrc`, file or the equivalent for your system/shell:

```export LUNUS_PATH="$PWD"```

```export PATH="${LUNUS_PATH}/c/bin/:${PATH}"```

- return to this directory: 

```$ cd -```
