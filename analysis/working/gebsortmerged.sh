#!/bin/sh

if [ $# -ne 1 ] 
  then
   echo "Specify only the run number to sort"
  exit 1
fi

RUN=$1

dir=/Users/heliosdigios/experiments

#cd $dir/GEBSort_working
GEBDIR=$dir/iss000/analysis/GEBSort
MERGDIR=$dir/iss000/merged_data
ROOTDIR=$dir/iss000/root_data
CHATDIR=$dir/iss000/analysis/working

echo "GEBSort started sorting run $RUN at `date`"
$GEBDIR/GEBSort_nogeb -input disk $MERGDIR/GEBMerged_run$RUN.gtd_000 -rootfile $ROOTDIR/run$RUN.root RECREATE -chat $CHATDIR/GEBSort.chat 
echo "GEBSort DONE at `date`"

#exit

