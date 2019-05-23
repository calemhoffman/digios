#!/bin/bash

if [ $# -eq 0 ] || ([ $1 != "start" ] && [ $1 != "stop" ]); then
    echo "Usage: ./elog.sh start/stop"
    exit 1
fi

option=$1

constFile=~/digios/expName.sh
source ${constFile} #load expName


if [ ${option} == "start" ]; then

    echo "pushing to elog"

    IDStr=$(elog -h www.phy.anl.gov -d elog -p 443 -l ${expName^^} -s -u GeneralHelios helios -a Category=Run -a RunNo=${LastRunNum} -a Subject="Run ${LastRunNum}" -n 1 -m ~/digios/analysis/working/elog.txt | tail -1 | awk '{print $4}')

    IDStr="elog"${IDStr}

    echo ${IDStr}

    echo $IDStr >> ${constFile}

fi


if [ ${option} == "stop" ]; then
    echo "pushing to elog"

    message=$(elog -h www.phy.anl.gov -d elog -p 443 -l ${expName^^} -s -u GeneralHelios helios -a Category=Run -a RunNo=${LastRunNum} -a Subject="Run ${LastRunNum}" -e ${elogID} -n 1 -m ~/digios/analysis/working/elog.txt)
    
    echo ${message}
fi