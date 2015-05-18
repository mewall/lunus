#!/bin/bash -f

. $1

if [ ! -d $work_dir ]; then
	mkdir $work_dir
fi

if [ ! -d $lattice_dir ]; then
	echo "Diffuse lattice directory $lattice_dir doesn't exist"
	return
fi

cd $work_dir

# initialize lattice sums

first_diffuse_file=`printf "%s/%s_diffuse_%05d.vtk" $lattice_dir $diffuse_lattice_prefix 1`

echo "First diffuse file is named $first_diffuse_file"

vtk2lat $first_diffuse_file tmp.lat 
constlt tmp.lat tmp_diffuse_sum.lat 0.0
constlt tmp.lat tmp_counts_sum.lat 0.0

for (( i=1; i<=$num_images; i++ ))

do

this_diffuse_file=`printf "%s/%s_diffuse_%05d.vtk" $lattice_dir $diffuse_lattice_prefix $i`

this_counts_file=`printf "%s/%s_counts_%05d.vtk" $lattice_dir $diffuse_lattice_prefix $i`

vtk2lat $this_diffuse_file tmp_diffuse.lat

vtk2lat $this_counts_file tmp_counts.lat

sumlt tmp_diffuse_sum.lat tmp_diffuse.lat tmp.lat
mv tmp.lat tmp_diffuse_sum.lat

sumlt tmp_counts_sum.lat tmp_counts.lat tmp.lat
mv tmp.lat tmp_counts_sum.lat

done

divlt tmp_diffuse_sum.lat tmp_counts_sum.lat tmp_mean.lat

mean_lattice_file=`printf "%s_mean.vtk" $diffuse_lattice_prefix`

lat2vtk tmp_mean.lat $mean_lattice_file

rm tmp_diffuse_sum.lat tmp_counts_sum.lat tmp_mean.lat
