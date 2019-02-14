#!/bin/bash -c
cd TestLunusDir
bash run_lunus.sh
anisolt lattices/TestLunus.lat lattices/TestLunus_aniso.lat
c=`corrlt lattices/TestLunus_aniso.lat ../../ref/TestLunusRef_aniso.lat`
#diff lattices/TestLunus.lat ../../ref/TestLunusRef.lat
cd -
if [ $c == 1 ]; then
  rslt=0
else
  rslt=1
fi
exit $rslt

