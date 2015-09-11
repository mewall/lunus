#!/bin/bash

. $1

if [ ! -d $work_dir ]; then
	mkdir $work_dir
fi

if [ ! -d $lattice_dir ]; then
	mkdir $lattice_dir
fi

cd $work_dir

if [ ! -d "scripts" ]; then
	mkdir scripts
fi

for (( i=1; i <= $num_images ; i=$i+1 ))

do

script_name=`printf script_integration_%05d.sh $i`

script_path=`printf scripts/%s $script_name`

this_diffuse_file=`printf "%s_diffuse_%05d.vtk" $diffuse_lattice_prefix $i`
this_counts_file=`printf "%s_counts_%05d.vtk" $diffuse_lattice_prefix $i`

cat > $script_path<<EOF

#!/bin/bash                         #-- what is the language of this shell
#                                  #-- Any line that starts with #$ is an instruction to SGE
#$ -S /bin/bash                     #-- the shell for the job
#$ -o $work_dir/scripts                        #-- output directory (fill in)
#$ -e $work_dir			#-- error directory (fill in)
#$ -cwd                            #-- tell the job that it should start in your working directory
#$ -r y                            #-- tell the system that if a job crashes, it should be restarted
#$ -j y                            #-- tell the system that the STDERR and STDOUT should be joined
#$ -l mem_free=1G                  #-- submits on nodes with enough free memory (required)
##$ -l xe5-2670=true
#$ -l arch=linux-x64               #-- SGE resources (CPU type)
#$ -l netapp=1G,scratch=1G         #-- SGE resources (home and scratch disks)
#$ -l h_rt=02:00:00                #-- runtime limit (see above; this requests 24 hours)
##$ -t 1-10                        #-- remove first '#' to specify the number of
                                   #-- tasks if desired (see Tips section)

# Anything under here can be a bash script

# If you used the -t option above, this same script will be run for each task,
# but with $SGE_TASK_ID set to a different value each time (1-10 in this case).
# The commands below are one way to select a different input (PDB codes in
# this example) for each task.  Note that the bash arrays are indexed from 0,
# while task IDs start at 1, so the first entry in the tasks array variable
# is simply a placeholder

#tasks=(0 1bac 2xyz 3ijk 4abc 5def 6ghi 7jkl 8mno 9pqr 1stu )
#input="${tasks[$SGE_TASK_ID]}"

date
hostname

qstat -j $JOB_ID                                  # This is useful for debugging and usage purposes,
							# e.g. "did my job exceed its memory request?"
#. proc.all

mkdir "tmpdir_"$i
cd "tmpdir_"$i

. $cctbx_dir/build/setpaths.sh

EOF

if [ -z ${resolution+x} ]; then 

cat >>$script_path<<EOF

cctbx.python ~/packages/lunus/scripts/integrate_diffuse_dials.py indexing.data=$indexing_data_file_one indexing.data=$indexing_data_file_two indexing.data=$indexing_data_file_three index_only=True cell.a=$cella cell.b=$cellb cell.c=$cellc inputlist.fname=$scales_input_file framenum=$i latxdim=$latxdim latydim=$latydim latzdim=$latzdim diffuse.lattice.type=sum diffuse.lattice.fname=$this_diffuse_file counts.lattice.fname=$this_counts_file np=1 codecamp.maxcell=$maxcell target_cell=$cella,$cellb,$cellc,$alpha,$beta,$gamma target_sg=$spacegroup 

EOF

else

cat >>$script_path<<EOF

cctbx.python ~/packages/lunus/scripts/integrate_diffuse_dials.py indexing.data=$indexing_data_file_one indexing.data=$indexing_data_file_two indexing.data=$indexing_data_file_three index_only=True cell.a=$cella cell.b=$cellb cell.c=$cellc inputlist.fname=$scales_input_file framenum=$i diffuse.lattice.resolution=$resolution diffuse.lattice.type=sum diffuse.lattice.fname=$this_diffuse_file counts.lattice.fname=$this_counts_file np=1 codecamp.maxcell=$maxcell target_cell=$cella,$cellb,$cellc,$alpha,$beta,$gamma target_sg=$spacegroup 

EOF

fi

cat >>$script_path<<EOF

mv $this_diffuse_file $lattice_dir/.
mv $this_counts_file $lattice_dir/.
cd ..
rm -r "tmpdir_"$i

date
EOF

#more $i_$y".pbs"

done
