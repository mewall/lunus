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

diffuse_glob=`printf "%s/%s_diffuse_*.npz" $lattice_dir $diffuse_lattice_prefix`
counts_glob=`printf "%s/%s_counts_*.npz" $lattice_dir $diffuse_lattice_prefix`
sum_fname_base=`printf "%s/%s_sum" $lattice_dir $diffuse_lattice_prefix`

script_name=script_sum.sh

script_path=`printf scripts/%s $script_name`

if [ -z ${setup_python+x} ]; then
  setup_python="module load python/2.7-anaconda-4.1.1"
fi

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
echo "SLURM_PROCID = \$SLURM_PROCID"

$setup_python

#qstat -j $JOB_ID                                  # This is useful for debugging and usage purposes,
							# e.g. "did my job exceed its memory request?"
#. proc.all

#. $phenix_dir/phenix_env.sh

EOF


if [ -z ${pphkl+x} ]; then 
  pphkl=1
fi

if [ -z ${filterhkl+x} ]; then 
  filterhkl=True
fi

if [ -z ${resolution+x} ]; then 

cat >>$script_path<<EOF

python $lunus_dir/scripts/integrate_lunus.py cell.a=$cella cell.b=$cellb cell.c=$cellc inputlist.fname=$scales_input_file framenum=$i latxdim=$latxdim latydim=$latydim latzdim=$latzdim diffuse.lattice.type=npz diffuse.lattice.glob=$this_diffuse_file counts.lattice.fname=$this_counts_file np=$nproc codecamp.maxcell=$maxcell target_cell=$cella,$cellb,$cellc,$alpha,$beta,$gamma target_sg=$spacegroup output.fname=$mean_lattice_file pphkl=$pphkl filterhkl=$filterhkl 

EOF

else

cat >>$script_path<<EOF

python $lunus_dir/scripts/calc_partial_sum_npz.py cell.a=$cella cell.b=$cellb cell.c=$cellc diffuse.lattice.resolution=$resolution diffuse.lattice.type=npz diffuse.lattice.glob=$diffuse_glob counts.lattice.glob=$counts_glob target_cell=$cella,$cellb,$cellc,$alpha,$beta,$gamma target_sg=$spacegroup output.fname.base=$sum_fname_base pphkl=$pphkl filterhkl=$filterhkl this.rank=\$SLURM_PROCID num.ranks=\$nranks
EOF

fi

cat >>$script_path<<EOF

#rm $diffuse_glob
#rm $counts_glob
#date
EOF

#more $i_$y".pbs"

