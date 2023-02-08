#!/usr/bin/env bash
# clean_up.sh
#
# removes all the files created in the running of the notebook
# for a fresh start

cd files_for_refinement
rm iobs_*
cd ../
rm starting_structure_refinement_against_experiment_*
rm single_structure_protein_only_refine_*
rm single_structure_protein_first_waters_refine_*
rm single_structure_protein_first_refinement_against_exp_*
rm ensemble.pdb
rm ensemble_cleaned.pdb
