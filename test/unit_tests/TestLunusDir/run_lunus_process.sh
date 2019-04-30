#!/bin/bash -c

export WORKDIR=${PWD}
sed 's@\${work_dir}@'${WORKDIR}'@g' lunus_params_unique.sh > lunus_input_params.sh
ts=`date +%s`
lunus.process experiments=raw/refined_experiments.json images=raw/snc_newhead_000??.img params=lunus_input_params.sh vtk=lattices/TestLunusProcess.vtk
vtk2lat lattices/TestLunusProcess.vtk lattices/TestLunusProcess.lat
te=`date +%s`
#dt_lunus=`echo "$te-$ts" | bc`
#echo "TIMING: Lunus = $dt_lunus sec"
