#!/bin/bash
#SBATCH -p general
#SBATCH --job-name=EBN_NUM
#SBATCH -N 1
#SBATCH -n 8
#SBATCH -e main2d.e.%J
#SBATCH -o main2d.o.%J
#SBATCH -t 7-0:0:0
#SBATCH --mem=64g

./main input2d.ss
