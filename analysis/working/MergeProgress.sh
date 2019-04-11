#!/bin/sh

if [ $# -eq 0 ]; then
   echo "please enter run number"
   exit 1
fi

RUN=$1

runLen=${#RUN}
if [ ${runLen} -eq 1 ]; then
   RUN="00"${RUN}
elif [ ${runLen} -eq 2 ]; then
   RUN="0"${RUN}
fi;

source ~/digios/expName.sh #load expName
AnalysisDir=~/digios/analysis

DATADIR=$AnalysisDir/data
MERGDIR=$AnalysisDir/merged_data

du -hsc $DATADIR/${expName}_run_${RUN}*

totalSize=`du -sc ${DATADIR}/${expName}_run_${RUN}.gtd* | tail -1| awk '{print $1}'`

mergedSize=`du -sc ${MERGDIR}/GEBMerged_run${RUN}.gtd* | tail -1| awk '{print $1}'`

echo "Total Size to merge : ${totalSize}"

haha=$(python -c "print(${mergedSize}*100/${totalSize})")

echo "Merged so far       : ${mergedSize}  [${haha}%]"
