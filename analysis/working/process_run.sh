#!/bin/sh

if [ $# -eq 0 ] ; then
  echo "$./process_run_simple.sh #RunNum #download #MergeSort #GeneralSort #Monitor"
  echo "              RunNum = a 3 digit run number, e.g. 001"
  echo "            download = 1/0, is download from DAQ?"
  echo "           MergeSort = 1/0, is Merge and Sort the raw data?"
  echo "          GenralSort = 2/1/0, is GeneralSort?  1 = GeneralSort.C, 2 = GeneralSortTrace.C"
  echo "             Monitor = 2/1/0, run ChainMonitor.C?  1 = single run, 2 = all runs"
  exit 1
fi;

source ../../expName.sh #load expName
AnalysisDir=../analysis

#remote data path
dataloc=/media/DIGIOSDATA3/${expName}/data
daqDir=/home/helios/digios

#===== directory and chat files
GEBDIR=$AnalysisDir/GEBSort
MERGDIR=$AnalysisDir/merged_data
ROOTDIR=$AnalysisDir/root_data
DATADIR=$AnalysisDir/data
MERGECHAT=$AnalysisDir/working/GEBMerge.chat
SORTCHAT=$AnalysisDir/working/GEBSort.chat


RUN=$1
isDownload=1
isMergeSort=1
isGeneralSort=1
isMonitor=1

if [ $# -ge 2 ]; then
 isDownload=$2
fi

if [ $# -ge 3 ]; then
 isMergeSort=$3
fi

if [ $# -ge 4 ]; then
 isGeneralSort=$4
fi

if [ $# -ge 5 ]; then
 isMonitor=$5
 OverRideMonitor=""
fi

if [ isGeneralSort -eq 2 ]; then
  isMonitor=0;
  OverRideMonitor="overrided by GeneralSortTrace."
fi

echo "============================================="
echo "============ RUN $RUN ========================"
echo "============================================="

echo "Download    : ${isDownload}"
echo "MergeSort   : ${isMergeSort}"
echo "GeneralSort : ${isGeneralSort}"
echo "isMonitor   : ${isMonitor} ${OverRideMonitor}"
echo "============================================="

if [ ${isDownload} -eq 1 ]; then
  echo "######################### Download raw data"
  PCName="$(hostname)"
  if [ ${PCName} == "digios1" ]; then
      echo "Already in digios1, no need to get data."
  else
      #============ Get the raw data
      IP=192.168.1.2
      echo "RUN $RUN: Get the raw data `date`"
      rsync -avuht --progress "helios@${IP}:${dataloc}/${exp}_run_$RUN.gtd*" ${DATADIR}/.
      rsync -avuht --progress "helios@${IP}:${daqDir}/RunTimeStamp.txt" ${AnalysisDir}/working/.
      echo "============================================="
      cat ${AnalysisDir}/working/RunTimeStamp.txt
      echo "============================================="

  fi
fi

du -hsc $DATADIR/${exp}_run_$RUN*

count=`ls -1 ${DATADIR}/${exp}_run_$RUN* 2>/dev/null | wc -l`
echo "========== Number of Files : ${count}"
if [ ${count} -eq 0 ]; then
    echo "============================================"
    echo "====  RAW Files of RUN-${RUN} not found! "
    echo "============================================"
    exit 
fi

#=========== Merge and Sort
if [ $isMergeSort -eq 1 ]; then
  echo "######################### Merge and Sort raw data"
  echo "RUN $RUN: GEBMerge started at `date`"
  $GEBDIR/GEBMerge $MERGECHAT  $MERGDIR/GEBMerged_run$RUN.gtd `ls $DATADIR/${exp}_run_$RUN.gtd*` > $MERGDIR/GEBMerge_run$RUN.log
  echo "RUN $RUN: GEBMerge DONE at `date`"

  echo "========= GEBSort started sorting run $RUN at `date`"
  $GEBDIR/GEBSort_nogeb -input disk $MERGDIR/GEBMerged_run$RUN.gtd_000 -rootfile $ROOTDIR/run$RUN.root RECREATE -chat $SORTCHAT 
  echo "GEBSort DONE at `date`"

  echo "========= saved root file -->  "  $ROOTDIR/run$RUN.root 

  echo "============================================="
  echo "============================================="
fi

#========== Process_run.C, GeneralSort

# convert to normal number, without zero in front
if [ "${RUN:0:1}" == "0" ] ; then
      runID=${RUN:1:2}
else
      runID=$( printf '%d' $RUN )
fi;


if [ $isGeneralSort -eq 1 ]; then
  echo "######################### GeneralSort.C"
  root -q -b "process_run.C(${runID})"
fi

if [ $isGeneralSort -eq 2 ]; then
  echo "######################### GeneralSortTrace.C"
  root -q -b "process_run.C(${runID}, 1)"
fi
root -l ../Armory/runsCheck.C  #check the run Entries, and duration

#=========== Monitor
if [ $isMonitor -eq 1 ] ; then
  root -l "ChainMonitors.C(${runID})"
fi
if [ $isMonitor -eq 2 ] ; then
  root -l ChainMonitors.C
fi;  
  
exit 1
