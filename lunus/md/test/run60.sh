python ../../command_line/lunusmd.py --topin water60.pdb --params analyze60.log --outfreq 10 --dt 0.0001 --temperature 300. --minsteps 1000 --mdsteps 10000 --elec_method ewald --trajout ../results/test_0001.pdb --enable-cuda --alpha 0.35 > ../results/test_0001.log
python ../../command_line/lunusmd.py --topin water60.pdb --params analyze60.log --outfreq 10 --dt 0.00005 --temperature 300. --minsteps 1000 --mdsteps 10000 --elec_method ewald --trajout ../results/test_00005.pdb --enable-cuda --alpha 0.35 > ../results/test_00005.log
grep Step ../results/test_0001.log | awk '{print $36}' |tail -500 > ../results/energy_0001.log
fluc1=$(grep Step ../results/test_0001.log | awk '{print $36}' |tail -500 | awk 'BEGIN{E=0.0;E2=0.0;ct=0}{E=E+$1;E2=E2+$1*$1;ct=ct+1}END{printf "%0.10f",E2/ct-E/ct*E/ct}')
grep Step ../results/test_00005.log | awk '{print $36}' |tail -500 > ../results/energy_00005.log
fluc2=$(grep Step ../results/test_00005.log | awk '{print $36}' |tail -500 | awk 'BEGIN{E=0.0;E2=0.0;ct=0}{E=E+$1;E2=E2+$1*$1;ct=ct+1}END{printf "%0.10f",E2/ct-E/ct*E/ct}')
r=$(echo "$fluc1/$fluc2" | bc -l)
echo "Energy fluctuations for 0.0001, 0.00005 = $fluc1, $fluc2 with ratio $r"


