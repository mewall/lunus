if [ "${LUNUS_MPI}" == "YES" ];
  mpirun -n 1 lunus test_inputs.sh
else
  lunus test_inputs.sh 
fi
diff ../proc/snc_newhead_lunus_00001.img ../ref/snc_newhead_lunus_00001.img
