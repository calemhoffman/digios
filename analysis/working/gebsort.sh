#!/bin/sh

if [ $# -ne 2 ] 
  then
   echo "Specify the run number and digitizer number to sort"
  exit 1
fi

RUN=$1
DIG=$2

dir=/Users/heliosdigios/experiments/infl001_19O

#cd $dir/GEBSort_working
GEBDIR=$dir/GEBSort
DATADIR=$dir/data
ROOTDIR=$dir/root_data
CHATDIR=$dir/working

echo "GEBSort started sorting run $RUN at `date`"
$GEBDIR/GEBSort_nogeb -input disk $DATADIR/infl_run_$RUN.gtd01_000_01$DIG -rootfile $ROOTDIR/single_run${RUN}_dig${DIG}.root RECREATE -chat $CHATDIR/GEBSort.chat 
echo "GEBSort DONE at `date`"

#exit

