#!/bin/sh

if [ $# -ne 2 ] ; then
   echo "Usage 1, single run for run#1: "
   echo "   $./process_run_MultiRuns.sh #1 #2 "
  exit 1
fi

exp=iss631
dir=/Users/heliosdigios/experiments/${exp}
#remote data path
dataloc=/media/DIGIOSDATA3/data/${exp}
daqDir=/home/helios/experiments/iss631

#===== directory and chat files
GEBDIR=$dir/GEBSort
MERGDIR=$dir/merged_data
ROOTDIR=$dir/root_data
DATADIR=$dir/data
MERGECHAT=$dir/working/GEBMerge.chat
SORTCHAT=$dir/working/GEBSort.chat

if [ "${${1}:0:1}" == "0" ] ; then
    RUN1=${${1}:1:2}
fi;
if [ "${${2}:0:1}" == "0" ] ; then
    RUN2=${${2}:1:2}
fi;

for iRUN in $(seq ${RUN1} ${RUN2})
do

  RUN=$(printf '%03d' ${iRUN})

  echo "============================================="
  echo "============ RUN $RUN ======================="
  echo "============================================="

  echo "RUN $RUN: Get the raw data `date`"


  rsync -avuht --progress "helios@anldaqrouter:${dataloc}/${exp}_run_$RUN.gtd*" ${DATADIR}/.
  rsync -avuht --progress "helios@anldaqrouter:${daqDir}/RunTimeStamp.txt" ${dir}/working/.
  echo "============================================"
  cat ${dir}/working/RunTimeStamp.txt
  echo "============================================"

  du -hsc $DATADIR/${exp}_run_$RUN*

  if [ ! -f $DATADIR/${exp}_run_$RUN.gtd01_000_0101 ]; then
      echo "============================================"
      echo "====  RAW Files of RUN-${RUN} not found! "
      echo "============================================"
      exit 
  fi

  echo "RUN $RUN: GEBMerge started at `date`"
  $GEBDIR/GEBMerge $MERGECHAT  $MERGDIR/GEBMerged_run$RUN.gtd `ls $DATADIR/${exp}_run_$RUN.gtd*` > $MERGDIR/GEBMerge_run$RUN.log
  echo "RUN $RUN: GEBMerge DONE at `date`"

  echo "GEBSort started sorting run $RUN at `date`"
  $GEBDIR/GEBSort_nogeb -input disk $MERGDIR/GEBMerged_run$RUN.gtd_000 -rootfile $ROOTDIR/run$RUN.root RECREATE -chat $SORTCHAT 
  echo "GEBSort DONE at `date`"

  echo "saved root file -->  "  $ROOTDIR/run$RUN.root 

  echo "============================================="
  echo "============================================="

  if [ "${RUN:0:1}" == "0" ] ; then
        #echo "octal"
        runID=${RUN:1:2}
  else
        #echo "decimal"
        runID=$( printf '%d' $RUN )
  fi;

  root -q -b "process_run.C($runID,0)"

done

root -l ../sort_codes/runsCheck.C
  
exit 1
