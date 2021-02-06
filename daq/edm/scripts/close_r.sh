#!/bin/bash -l
source ${HELIOSSYS}/expName.sh #load expName
export TERM=vt100
explocation=${HELIOSSYS}/daq
datalocation=${daqDataPath}/${expName}/data

echo -e "############################### Closing RUN : RUN\033[0;31m${LastRunNum}\033[0m"

cd ${datalocation}
xterm -T stopdata -fa "Monospace" -fs 12 -bg rgb:7f/99/ff -fg white -geometry 130x35+500+20 -e "${explocation}/edm/scripts/stop_run.sh"
#echo "Pause for a 2 seconds after stopped run."
#sleep 2


#========= check is autorun
autoRunID=$(ps -e | grep "AutoStartStop" | awk '{print $1}')
kill -9 $autoRunID

#globus task list

cd ${explocation}
echo "Terminal is now Free For Use"
echo "###############################################"
exit
