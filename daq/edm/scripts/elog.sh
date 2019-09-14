#!/bin/bash

if [ $# -eq 0 ] || ([ $1 != "start" ] && [ $1 != "stop" ]); then
    echo "Usage: ./elog.sh start/stop"
    exit 1
fi

option=$1

constFile=~/digios/expName.sh
source ${constFile} #load expName

#echo ${expName}
echo "H"${expName:1}

if [ ${option} == "start" ]; then

    echo "pushing to elog for started a run"

    #IDStr=$(elog -h www.phy.anl.gov -d elog -p 443 -l "H"${expName:1} -s -u GeneralHelios helios -a Category=Run -a RunNo=${LastRunNum} -a Subject="Start Run ${LastRunNum}" -n 1 -m ~/digios/analysis/working/elog.txt)

    IDStr=$(elog -h websrv1.phy.anl.gov -p 8080 -l "H"${expName:1} -u GeneralHelios helios -a Category=Run -a RunNo=${LastRunNum} -a Subject="Start Run ${LastRunNum}" -n 1 -m ~/digios/analysis/working/elog.txt)

    echo ${IDStr}

    IDStr=$(${IDStr} | tail -1 | awk '{print $4}')

    echo ${IDStr}

    #need to check ID is valid. other wise it crash other script
    re='^[0-9]+$'
    if [ ${IDStr:0:3} == "ID=" ] && [[ ${IDStr:3} =~ ${re} ]]; then
	elogIDStr="elog"${IDStr}
	echo "Elog is succefully pushed at ${elogIDStr}"
	echo ${elogIDStr} >> ${constFile}
    fi

fi


if [ ${option} == "stop" ]; then
    echo "pushing to elog for stopped a run"

    #message=$(elog -h www.phy.anl.gov -d elog -p 443 -l ${expName^^} -s -u GeneralHelios helios -a Category=Run -a RunNo=${LastRunNum} -a Subject="Stop Run ${LastRunNum}" -n 1 -m ~/digios/analysis/working/elog.txt)

    screenShot=~/digios/analysis/working/grafanaElog.jpg

    message=$(elog -h websrv1.phy.anl.gov -p 8080 -l "H"${expName:1} -u GeneralHelios helios -a Category=Run -a RunNo=${LastRunNum} -a Subject="Start Run ${LastRunNum}" -n 1 -m ~/digios/analysis/working/elog.txt -f ${screenShot})

    
    echo ${message}


fi
