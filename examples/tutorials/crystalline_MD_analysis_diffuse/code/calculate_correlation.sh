#!/bin/bash

exp=$1
model=$2

#############################
# PROCESS EXPERIMENTAL DATA #
#############################

# cull experimental data to match simulated data resolution (1.6 A)
lunus.cullreslt $exp ${exp%.lat}_culled.lat 200 1.6
# symmetrize
lunus.symlt ${exp%.lat}_culled.lat ${exp%.lat}_culled_sym.lat -3
# calculated experimental data anisotropic component
lunus.anisolt ${exp%.lat}_culled_sym.lat ${exp%.lat}_culled_sym_aniso.lat $(cat cell.txt)
# calculate isotropic profile for experimental data
lunus.avgrlt ${exp%.lat}_culled.lat ${exp%.lat}_culled_iso.rf $(cat cell.txt)
lunus.binasc 2 < ${exp%.lat}_culled_iso.rf > ${exp%.lat}_culled_iso.dat

######################
# PROCESS MODEL DATA #
######################

# cull the model data to 1.6 A
lunus.cullreslt $model ${model%.lat}_culled.lat 200 1.6
# symmetrize simulated diffuse scattering fro P41 space group
lunus.symlt ${model%.lat}_culled.lat ${model%.lat}_culled_sym.lat -3;
# calculate isotropic profile for simulated data
lunus.avgrlt ${model%.lat}_culled_sym.lat ${model%.lat}_culled_sym_iso.rf $(cat cell.txt)
lunus.binasc 2 < ${model%.lat}_culled_sym_iso.rf > ${model%.lat}_culled_sym_iso.dat
# calculate anisotropic component of simulated ata
lunus.anisolt ${model%.lat}_culled_sym.lat ${model%.lat}_culled_sym_aniso.lat $(cat cell.txt)

########################
# FULL MAP CORRELATION #
########################

# calculate the full diffuse map correlation coefficient
lunus.corrlt ${model%.lat}_culled_sym.lat ${exp%.lat}_culled_sym.lat > full_map_correlation_coefficient.txt

#######################
# CORRELATION IN BINS #
#######################

# calculate the correlation coefficient for the anisotropic in bins
lunus.ccrlt ${model%.lat}_culled_sym_aniso.lat ${exp%.lat}_culled_sym_aniso.lat anisotropic_correlation_in_bins.rf $(cat cell.txt)
lunus.binasc 2 < anisotropic_correlation_in_bins.rf > anisotropic_correlation_in_bins.dat
# calculate the full correrlation coefficient for the anisotropic maps
lunus.corrlt ${model%.lat}_culled_sym_aniso.lat ${exp%.lat}_culled_sym_aniso.lat > anisotropic_component_correlation_coefficient.txt
