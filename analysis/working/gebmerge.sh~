#!/bin/sh

if [ $# -ne 1 ] 
  then
   echo "Specify only the run number to sort"
  exit 1
fi

RUN=$1

dir=/Users/heliosdigios/experiments/infl001_19O

echo "RUN $RUN: GEBMerge started at `date`"
 
$dir/GEBSort/GEBMerge $dir/working/GEBMerge.chat  $dir/merged_data/GEBMerged_run$RUN.gtd `ls $dir/data/infl_run_$RUN.gtd*`
 
echo "RUN $RUN: GEBMerge DONE at `date`"
