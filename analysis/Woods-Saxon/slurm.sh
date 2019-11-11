#!/bin/bash

#SBATCH -J WS17O
#SBATCH -p bdwall
#SBATCH -N 1 
#SBATCH --ntasks-per-node=36 
#SBATCH -t 12:00:00

root -b 'ProofWSSearch.C+("17O",1,"wsSearch_FineRange.txt","ws_17O_all.root")'
