if [ "${LUNUS_MPI}" == "YES" ]; then
  if [ -z ${SLURM_NODELIST+x} ]; then
    mpirun -n 1 lunus test_inputs.sh
  else
    srun -n 1 lunus test_inputs.sh
  fi
else
  lunus test_inputs.sh 
fi
diff ../proc/snc_newhead_lunus_00001.img ../ref/snc_newhead_lunus_00001.img
