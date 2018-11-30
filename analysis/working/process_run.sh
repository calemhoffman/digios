#!/bin/sh

if [ $# -eq 0 ] ; then
   echo "$./process_run_simple.sh #RUNNUM "
   echo "$./process_run_simple.sh #RUNNUM #Option_for_single_run"
                                          #If option = 1, run all processed runs, else, only process this run
  exit 1
fi;

exp=iss631
AnalysisDir=/Users/ryantang/Desktop/HELIOS_repos/analysis

#remote data path
dataloc=/media/DIGIOSDATA3/data/${exp}
daqDir=/home/helios/experiments/iss631

#===== directory and chat files
GEBDIR=$AnalysisDir/GEBSort
MERGDIR=$AnalysisDir/merged_data
ROOTDIR=$AnalysisDir/root_data
DATADIR=$AnalysisDir/data
MERGECHAT=$AnalysisDir/working/GEBMerge.chat
SORTCHAT=$AnalysisDir/working/GEBSort.chat


RUN=$1

echo "============================================="
echo "============ RUN $RUN ======================="
echo "============================================="

if [ $# -eq 2 ] ; then
  echo "=================== single run ."
fi

#============ Get the raw data
echo "RUN $RUN: Get the raw data `date`"
rsync -avuht --progress "helios@anldaqrouter:${dataloc}/${exp}_run_$RUN.gtd*" ${DATADIR}/.
rsync -avuht --progress "helios@anldaqrouter:${daqDir}/RunTimeStamp.txt" ${AnalysisDir}/working/.
echo "============================================="
cat ${AnalysisDir}/working/RunTimeStamp.txt
echo "============================================="

du -hsc $DATADIR/${exp}_run_$RUN*

if [ ! -f $DATADIR/${exp}_run_$RUN.gtd03_000_0110 ]; then
    echo "============================================"
    echo "====  RAW Files of RUN-${RUN} not found! "
    echo "============================================"
    exit 
fi;

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

#========== convert to normal number, without zero in front
if [ "${RUN:0:1}" == "0" ] ; then
      runID=${RUN:1:2}
else
      runID=$( printf '%d' $RUN )
fi;

root -q -b "process_run.C($runID,0)"
root -l ../sort_codes/runsCheck.C

#=========== If option = 1, run all processed runs, else, only process this run
if [ $# -eq 1 ] ; then
  root -l ChainMonitors.C
else
  root -l "ChainMonitors.C($runID)"
fi;
  
exit 1
