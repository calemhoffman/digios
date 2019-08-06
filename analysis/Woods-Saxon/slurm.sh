#!/bin/bash

#SBATCH -J WS17O
#SBATCH -p bdwall
#SBATCH -N 1 
#SBATCH --ntasks-per-node=36 
#SBATCH -t 00:05:00

root -b 'ProofWSSearch.C+("17O",1,"wsSearch_test.txt","ws_17O.root")'
