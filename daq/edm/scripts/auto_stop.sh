#!/bin/bash

echo ""
echo "###################################"
echo "        Auto Stop script           "
echo "###################################"

RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

source ~/digios/expName.sh
RUN=${LastRunNum}

stopFlag=0

maxSize=3145728 #3GB
#maxSize=5000000 #~5GB

count=0

while [  $stopFlag -eq 0 ]; do  

    size=$(du -c --time ~/digios/analysis/data/${expName}_run_*$RUN* | tail -1 | awk '{print $1}')

    if [ $size -gt $maxSize ]; then
	stopFlag=1
	echo $(date) " | Run" ${RUN} " | size:" $size " > " $maxSize " ====> Stop!!!"	
	source ~/digios/daq/edm/scripts/stop_run.sh
    else
	
	echo $(date) " | Run" ${RUN} " | size:" $size " < " $maxSize

    fi

#    count=$((count+1))
#    if [ $count -eq 6 ]; then 
#	stopFlag=1
#	source ~/digios/daq/edm/scripts/stop_run.sh
#    fi

    sleep 20

done

