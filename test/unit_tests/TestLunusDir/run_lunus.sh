#!/bin/bash

export WORKDIR=${PWD}
sed 's@\${work_dir}@'${WORKDIR}'@g' lunus_params_unique.sh > lunus_input_params.sh
ts=`date +%s`
lunus lunus_input_params.sh
te=`date +%s`
dt_lunus=`echo "$te-$ts" | bc`
echo "TIMING: Lunus = $dt_lunus sec"
