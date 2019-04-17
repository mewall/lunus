#!/bin/bash -c

export WORKDIR=${PWD}
sed 's@\${work_dir}@'${WORKDIR}'@g' lunus_params_unique.sh > lunus_input_params.sh
#sed 's@\${work_dir}@'${WORKDIR}'@g' jsonlist_template.inp > jsonlist.inp
for (( i=1;i<=10;i++ )) 
do
  if=`printf "json_files/experiments_for_lunus_%05d_template.json" $i`
  of=`printf "json_files/experiments_for_lunus_%05d.json" $i`
  sed 's@\${work_dir}@'${WORKDIR}'@g' $if > $of
done
ts=`date +%s`
if [ "${LUNUS_MPI}" == "YES" ]; then
  if [ -z ${SLURM_NODELIST+x} ]; then
    mpirun -np 4 lunus lunus_input_params.sh
  else
    srun -n 4 lunus lunus_input_params.sh
  fi
else
  lunus lunus_input_params.sh 
fi
te=`date +%s`
#dt_lunus=`echo "$te-$ts" | bc`
#echo "TIMING: Lunus = $dt_lunus sec"
