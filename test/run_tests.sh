cd unit_tests
sed 's@\${LUNUS_HOME}@'${LUNUS_HOME}'@g' lunus_params_unique.sh > test_inputs.sh 
#cat test_inputs.sh
rslt=0
for test in Test*.sh; do
#  bash $test >& /dev/null
  bash $test 
  if [ $? == 0 ]; then
    echo "$test: Passed"
  else
    echo "$test: Failed"
    rslt=1
  fi
done
cd - >& /dev/null
exit $rslt
