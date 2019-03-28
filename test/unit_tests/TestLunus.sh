#!/bin/bash -c
cd TestLunusDir
bash run_lunus.sh
anisolt lattices/TestLunus.lat lattices/TestLunus_aniso.lat
c=`corrlt lattices/TestLunus_aniso.lat ../../ref/TestLunusRef_aniso.lat`
#diff lattices/TestLunus.lat ../../ref/TestLunusRef.lat
cd -
if (( $(awk -v n1="$c" -v n2="0.99999" 'BEGIN {print (n1<n2?"0":"1")}') )); then
  rslt=0
else
  echo "Failed correlation test, c = $c"
  rslt=1
fi
exit $rslt

