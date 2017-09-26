#!/bin/bash

. $1

if [ -z ${image_suffix+x} ]; then echo "image_suffix will be set to cbf";image_suffix=cbf; else echo "image_suffix is set to '$image_suffix'"; fi

if [ -z ${maxcell+x} ]; then 
    maxcell=-1
fi

if [ ! -d $work_dir ]; then
	mkdir $work_dir
fi

if [ ! -d $lattice_dir ]; then
	mkdir $lattice_dir
fi

cd $work_dir

if [ ! -d "$work_dir/scripts" ]; then
	mkdir $work_dir/scripts
fi

for (( i=1; i <= $num_images ; i++ ))

do

integration_image_name=`printf %s_%05d.%s $integration_image_prefix $i $image_suffix`

integration_image_path=`printf %s/%s $lunus_image_dir $integration_image_name`

echo "$i $integration_image_path 1.0 0.0" >> $scales_output_file 

done

# Path to the correction factor image

if [ ! -d "$work_dir/scripts/tmpdir_common" ]; then
   mkdir $work_dir/scripts/tmpdir_common
fi

# Use indexing file number one as the template correction factor image

template_image_path="$indexing_data_file_one"

script_name=`printf script_index.sh $i`

script_path=`printf scripts/%s $script_name`

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

#qstat -j $JOB_ID                                  # This is useful for debugging and usage purposes,
							# e.g. "did my job exceed its memory request?"
#. proc.all

. $cctbx_dir/setpaths_all.sh
#. $phenix_dir/phenix_env.sh


cfim $template_image_path $work_dir/scripts/tmpdir_common/correction.imf $polarim_dist $polarim_polarization $polarim_offset

EOF

cat >>$script_path<<EOF

time libtbx.python $lunus_dir/scripts/index_cctbx.py libtbx.modules.path=$cctbx_dir/modules indexing.data=$indexing_data_file_one indexing.data=$indexing_data_file_two indexing.data=$indexing_data_file_three inputlist.fname=$scales_input_file np=1 framenum=-1 maxcell=$maxcell target_cell=$cella,$cellb,$cellc,$alpha,$beta,$gamma target_sg=$spacegroup 

#time libtbx.python -m cProfile $lunus_dir/scripts/index_cctbx.py indexing.data=$indexing_data_file_one indexing.data=$indexing_data_file_two indexing.data=$indexing_data_file_three inputlist.fname=$scales_input_file np=1 framenum=-1 codecamp.maxcell=$maxcell target_cell=$cella,$cellb,$cellc,$alpha,$beta,$gamma target_sg=$spacegroup 

EOF

cat >>$script_path<<EOF


date
EOF

#more $i_$y".pbs"

