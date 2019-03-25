#!/bin/bash
#SBATCH -J WS209Pb
#SBATCH -p bdwall
#SBATCH -N 1
#SBATCH --ntasks-per-node=36
#SBATCH -t 1:00:00

root -b ProofWSSearch.C+

