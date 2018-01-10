#!/bin/bash
#SBATCH --time=00:10:00
#SBATCH -N 64
#SBATCH --constraint=haswell

#module load friendly-testing
#module swap intel/16.0.3 intel/17.0.0
#module load intel-performance-tools/2017.0.031
#module swap craype-haswell craype-mic-knl

export WORKDIR=${PWD}
export OMP_NUM_THREADS=1

tasks_per_node=4

cd $WORKDIR
if [ -e genlat.input ]; then
    rm genlat.input
fi
if [ -e scripts ]; then
    rm -rf scripts
fi
sed 's@\${work_dir}@'${WORKDIR}'@g' lunus_params_unique.sh > lunus_input_params.sh
srun --nodes=1 bash /lustre/ttscratch1/mewall/gitlab/lunus/scripts/make_index_script.sh lunus_input_params.sh
#srun --nodes=1 bash /lustre/ttscratch1/mewall/gitlab/lunus/scripts/make_lunus_scripts.sh lunus_input_params.sh
#cd scripts
#ls -1 script_lunus_*.sh > file_list
#split -n l/$SLURM_JOB_NUM_NODES -d -a 4 file_list
#cd -
export OMP_NUM_THREADS=16
ts=`date +%s`
srun --nodes=$SLURM_JOB_NUM_NODES --tasks-per-node=$tasks_per_node --cpus-per-task=$OMP_NUM_THREADS --hint=multithread lunus lunus_input_params.sh
#srun --nodes=$SLURM_JOB_NUM_NODES --tasks-per-node=1 --cpus-per-task=64 --hint=multithread bash parallel_commands.sh 4 16
te=`date +%s`
dt_lunus=`echo "$te-$ts" | bc`
echo "TIMING: Image processing scripts = $dt_lunus sec"
export OMP_NUM_THREADS=1
date
#ts=`date +%s`
#aprun bash /lustre/ttscratch1/mewall/gitlab/lunus/scripts/calc_image_scale_factors.sh lunus_params_unique.sh
#te=`date +%s`
#dt_lunus=`echo "$te-$ts" | bc`
#echo "TIMING: Scale factors = $dt_lunus sec"
cd scripts
ts=`date +%s`
srun --nodes=1 bash script_index.sh
te=`date +%s`
dt_index=`echo "$te-$ts" | bc`
echo "TIMING: CCTBX indexing = $dt_index sec"
cd -
srun --nodes=1 bash /lustre/ttscratch1/mewall/gitlab/lunus/scripts/make_integrate_lunus_scripts.sh lunus_input_params.sh
cd scripts
ls -1 script_integration_*.sh > file_list
split -n l/$SLURM_JOB_NUM_NODES -d -a 4 file_list
cd -
ts=`date +%s`
srun --nodes=$SLURM_JOB_NUM_NODES --tasks-per-node=1 --cpus-per-task=64 --hint=multithread bash parallel_commands.sh 16 4
#aprun -cc depth -n 7 -N 1 -d 32 -j 1 bash parallel_commands.sh 32 1
te=`date +%s`
dt_lunus=`echo "$te-$ts" | bc`
echo "TIMING: Integration scripts = $dt_lunus sec"
ts=`date +%s`
#aprun bash /lustre/ttscratch1/mewall/gitlab/lunus/scripts/calc_mean_lattice.sh lunus_params_unique_2000.sh
srun --nodes=1 bash /lustre/ttscratch1/mewall/gitlab/lunus/scripts/make_calc_mean_script.sh lunus_input_params.sh
srun --nodes=1 bash /lustre/ttscratch1/mewall/gitlab/lunus/scripts/make_calc_partial_sum_script.sh lunus_input_params.sh
export num_images=`sed -n 's/^num_images=\([0-9]*$\)/\1/p' lunus_input_params.sh`
export nranks=`echo "sqrt(${num_images})+1" | bc`
echo "DEBUG: Number of images=${num_images}; number of ranks = ${nranks}"
export tasks_per_node=4
export OMP_NUM_THREADS=16
cd scripts
#export nranks=`echo "$SLURM_JOB_NUM_NODES * $tasks_per_node" | bc`
srun --nodes=$SLURM_JOB_NUM_NODES --tasks-per-node=$tasks_per_node --cpus-per-task=$OMP_NUM_THREADS --hint=multithread bash script_sum.sh
srun --nodes=1 bash script_mean.sh
cd -
te=`date +%s`
dt_lunus=`echo "$te-$ts" | bc`
echo "TIMING: Reduction = $dt_lunus sec"
