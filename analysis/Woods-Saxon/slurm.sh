#!/bin/bash
#SBATCH -J WS209PbFine
#SBATCH -p bdwall
#SBATCH -N 1
#SBATCH --ntasks-per-node=1
#SBATCH -t 11:00:00

./WSSearch energy209Pb.dat 209 wsSearch_FineRange.txt ws_209Pb_Fine.root
#./WSSearch energy211Po.dat 211 wsSearch_CorseRange.txt ws_211Po.root
#./WSSearch energy207Hg.dat 207 wsSearch_CorseRange.txt ws_207Hg.root
