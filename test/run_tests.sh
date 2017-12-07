cd unit_tests
sed 's@\${LUNUS_HOME}@'${LUNUS_HOME}'@g' lunus_params_unique.sh > test_inputs.sh 
cat test_inputs.sh
for test in Test*.sh; do
  bash $test >& /dev/null
  if [ $? == 0 ]; then
    echo "$test: Passed"
  else
    echo "$test: Failed"
  fi
done
cd - >& /dev/null
