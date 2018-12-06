#!/bin/sh

if [ $# -ne 2 ] ; then
   echo "Usage : "
   echo "   $./process_run_MultiRuns.sh #1 #2 "
  exit 1
fi

exp=iss631
AnalysisDir=../../analysis

#remote data path
dataloc=/media/DIGIOSDATA3/data/${exp}
daqDir=/home/helios/experiments/${exp}

#===== directory and chat files
GEBDIR=$AnalysisDir/GEBSort
MERGDIR=$AnalysisDir/merged_data
ROOTDIR=$AnalysisDir/root_data
DATADIR=$AnalysisDir/data
MERGECHAT=$AnalysisDir/working/GEBMerge.chat
SORTCHAT=$AnalysisDir/working/GEBSort.chat

RUN1=$1
RUN2=$2

if [ "${RUN1:0:1}" == "0" ] ; then
      RUN1=${RUN1:1:2}
else
      RUN1=$( printf '%d' $RUN1 )
fi;

if [ "${RUN2:0:1}" == "0" ] ; then
      RUN2=${RUN2:1:2}
else
      RUN2=$( printf '%d' $RUN2 )
fi;

for iRUN in $(seq ${RUN1} ${RUN2})
do

  RUN=$(printf '%03d' ${iRUN})

  echo "============================================="
  echo "============ RUN $RUN ======================="
  echo "============================================="

  echo "RUN $RUN: Get the raw data `date`"

  #============ Get the raw data
  rsync -avuht --progress "helios@anldaqrouter:${dataloc}/${exp}_run_$RUN.gtd*" ${DATADIR}/.
  rsync -avuht --progress "helios@anldaqrouter:${daqDir}/RunTimeStamp.txt" ${AnalysisDir}/working/.
  echo "============================================"
  cat ${AnalysisDir}/working/RunTimeStamp.txt
  echo "============================================"

  du -hsc $DATADIR/${exp}_run_$RUN*

  count=`ls -1 ${DATADIR}/${exp}_run_$RUN* 2>/dev/null | wc -l`
  if [$count == 0 ]; then
      echo "============================================"
      echo "====  RAW Files of RUN-${RUN} not found! "
      echo "============================================"
      exit 
  fi

  #=========== Merge and Sort
  echo "RUN $RUN: GEBMerge started at `date`"
  $GEBDIR/GEBMerge $MERGECHAT  $MERGDIR/GEBMerged_run$RUN.gtd `ls $DATADIR/${exp}_run_$RUN.gtd*` > $MERGDIR/GEBMerge_run$RUN.log
  echo "RUN $RUN: GEBMerge DONE at `date`"

  echo "GEBSort started sorting run $RUN at `date`"
  $GEBDIR/GEBSort_nogeb -input disk $MERGDIR/GEBMerged_run$RUN.gtd_000 -rootfile $ROOTDIR/run$RUN.root RECREATE -chat $SORTCHAT 
  echo "GEBSort DONE at `date`"

  echo "saved root file -->  "  $ROOTDIR/run$RUN.root 

  echo "============================================="
  echo "============================================="

  #========== Process_run.C, GeneralSort
  if [ "${RUN:0:1}" == "0" ] ; then
        runID=${RUN:1:2}
  else
        runID=$( printf '%d' $RUN )
  fi;

  root -q -b "process_run.C($runID,0)"

done

root -l ../Armory/runsCheck.C
  
exit 1
