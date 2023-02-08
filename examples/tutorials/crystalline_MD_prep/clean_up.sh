#!/usr/bin/env bash
# clean_up.sh
#
# removes all the files created in the running of the notebook
# for a fresh start

rm -r equilibration_2/
rm mdout.mdp
rm -r minimization_2/
rm supercell.top
rm supercell_solvated_neutral_2.gro
rm -r equilibration_1/
rm -r minimization_1/
rm supercell_solvated_neutral.gro
rm ions.tpr
rm supercell_solvated.gro
rm supercell_no_chains_renumbered.pdb
rm supercell.pdb
rm \#supercell.top.*
rm asymmetric_unit.pdb
rm protein_calcium_gmx.pdb
rm protein.top
rm protein_Ion2.itp
rm posre_Ion2.itp
rm protein_Protein.itp
rm posre_Protein.itp
rm -r pdb4amber_files/
rm protein_calcium_amber.pdb
rm protein_calcium.pdb
rm pdTp.pdb

# remove any additional gromacs
# saved files
if [ -f "\#*" ]; then
    rm \#*
fi

# remove any gromacs step files
# from MD runs
if [ -f "step*" ]; then
    rm step*
fi
