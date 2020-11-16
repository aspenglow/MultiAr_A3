#!/bin/bash
#SBATCH --chdir /home/dawang/A3
#SBATCH --nodes 1
#SBATCH --ntasks 1
#SBATCH --cpus-per-task 28
#SBATCH --mem 10G


echo STARTING AT `date`

numactl --cpunodebind=0 --membind=1 ./numa
numactl --interleave=all ./numa
numactl --localalloc ./numa

echo FINISHED at `date`
