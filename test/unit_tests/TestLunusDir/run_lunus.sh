#!/bin/bash

export WORKDIR=${PWD}
sed 's@\${work_dir}@'${WORKDIR}'@g' lunus_params_unique.sh > lunus_input_params.sh
ts=`date +%s`
if [ "${LUNUS_MPI}" == "YES" ]; then
  if [ -z ${SLURM_NODELIST+x} ]; then
    mpirun -n 1 lunus lunus_input_params.sh
  else
    srun -n 1 lunus lunus_input_params.sh
  fi
else
  lunus lunus_input_params.sh 
fi
te=`date +%s`
dt_lunus=`echo "$te-$ts" | bc`
echo "TIMING: Lunus = $dt_lunus sec"
