#!/bin/bash -c
cd TestLunusDir
bash run_lunus_process.sh
anisolt lattices/TestLunusProcess.lat lattices/TestLunusProcess_aniso.lat
c=`corrlt lattices/TestLunusProcess_aniso.lat ../../ref/TestLunusRef_aniso.lat`
#diff lattices/TestLunus.lat ../../ref/TestLunusRef.lat
cd -
if (( $(awk -v n1="$c" -v n2="0.999" 'BEGIN {print (n1<n2?"0":"1")}') )); then
  rslt=0
else
  echo "Failed correlation test, c = $c"
  rslt=1
fi
exit $rslt

