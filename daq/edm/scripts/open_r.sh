#!/bin/bash -l
export TERM=vt100
explocation=~/digios/daq
source ~/digios/expName.sh #load expName
datalocation=${daqDataPath}/${expName}/data

newRunNum=$((LastRunNum+1))
echo "############################### Start a new RUN : RUN\033[0;31m${newRunNum}\033[0m"

echo $datalocation

cd ${datalocation}
caput Online_CS_SaveData No Save
caput Online_CS_StartStop Stop
xterm -T savedata -fa "Monospace" -fs 12 -bg rgb:20/9d/20 -fg white -geometry 100x30+500+20 -e "${explocation}/edm/scripts/start_run.sh"
sleep 1
cd ${explocation}

echo Run is underway
echo Done - Terminal Window is Free to use 
