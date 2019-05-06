#!/bin/bash
lat2hkl ${1}.lat tmp.hkl
anisolt ${1}.lat tmp_aniso.lat $2
lat2hkl tmp_aniso.lat tmp_aniso.hkl
m=`awk 'BEGIN{m=0}{if (m>$4) {m=$4}}END{print m}' tmp.hkl`
echo $m
awk -v m="$m" -v cell="$2" -v sg="$3" 'BEGIN{split(cell,a,",");printf "    1\n -987\n% 10.3f% 10.3f% 10.3f% 10.3f% 10.3f% 10.3f %s\n",a[1],a[2],a[3],a[4],a[5],a[6],sg}{printf "%4d%4d%4d%8.1f%8.1f\n",$1,$2,$3,$4-m,sqrt($4-m)}' tmp.hkl > tmp.sca
m=`awk 'BEGIN{m=0}{if (m>$4) {m=$4}}END{print m}' tmp_aniso.hkl`
echo $m
awk -v m="$m" -v cell="$2" -v sg="$3" 'BEGIN{split(cell,a,",");printf "    1\n -987\n% 10.3f% 10.3f% 10.3f% 10.3f% 10.3f% 10.3f %s\n",a[1],a[2],a[3],a[4],a[5],a[6],sg}{printf "%4d%4d%4d%8.1f%8.1f\n",$1,$2,$3,$4-m,sqrt($4-m)}' tmp_aniso.hkl > tmp_aniso.sca
echo "Total Intentisy Statistics:"
phenix.merging_statistics file_name=tmp.sca n_bins=20 high_resolution=$4
echo "Anisotropic Intensity Statistics:"
phenix.merging_statistics file_name=tmp_aniso.sca n_bins=20 high_resolution=$4
