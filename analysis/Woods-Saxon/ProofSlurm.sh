#!/bin/bash
#SBATCH -J WS17O
#SBATCH -p bdwall
#SBATCH -N 1
#SBATCH --ntasks-per-node=36
#SBATCH -t 00:02:30

root -b ProofWSSearch.C+

