#!/bin/bash -l
export TERM=vt100
explocation=${HELIOSSYS}/daq
source ${HELIOSSYS}/expName.sh #load expName
datalocation=${daqDataPath}/${expName}/data

if [ $# -eq 1 ]; then
    min=$1
    echo "Start Auto Run for ${min} min each"
fi

#=== check AutoStartStop is running?
IsAuto=$(ps -e | grep "AutoStartStop" | wc -l)
if [ $IsAuto -ge 1 ]; then
    echo -e "\033[0;33m !!!!!!!!!! Auto start stop is runing. Abort !!!!!!!! \033[0m"
    exit
fi 


newRunNum=$((LastRunNum+1))
echo -e "############################### Start a new RUN : RUN\033[0;31m${newRunNum}\033[0m"

echo $datalocation
cd ${datalocation}

if [ $# -eq 1 ]; then 
    min=$1
    xterm -T savedata -fa "Monospace" -fs 12 -bg rgb:00/00/00 -fg white -geometry 100x30+500+20 -e "${explocation}/edm/scripts/AutoStartStop time ${min} -1"
else
    caput Online_CS_SaveData No Save
    caput Online_CS_StartStop Stop
    xterm -T savedata -fa "Monospace" -fs 12 -bg rgb:20/9d/20 -fg white -geometry 100x30+500+20 -e "${explocation}/edm/scripts/start_run.sh"
fi

sleep 1
cd ${explocation}

echo "Done - Terminal Window is Free to use "
echo "###############################################"
