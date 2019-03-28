#!/bin/bash -c
cd TestLunusDir
bash run_lunus.sh
anisolt lattices/TestLunus.lat lattices/TestLunus_aniso.lat
c=`corrlt lattices/TestLunus_aniso.lat ../../ref/TestLunusRef_aniso.lat`
#diff lattices/TestLunus.lat ../../ref/TestLunusRef.lat
cd -
if (( $(echo "$c >= 0.999999" | bc -l) )); then
  rslt=0
else
  echo "Failed correlation test, c = $c"
  rslt=1
fi
exit $rslt

