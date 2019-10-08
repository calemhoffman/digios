#!/bin/bash -l
source ~/digios/expName.sh #load expName
export TERM=vt100
explocation=~/digios/daq
datalocation=${daqDataPath}/${expName}/data

echo -e "############################### Closing RUN : RUN\033[0;31m${LastRunNum}\033[0m"


cd ${datalocation}
xterm -T stopdata -fa "Monospace" -fs 12 -bg rgb:7f/99/ff -fg white -geometry 130x35+500+20 -e "${explocation}/edm/scripts/stop_run.sh"
#echo "Pause for a 2 seconds after stopped run."
#sleep 2
cd ${explocation}
echo "Terminal is now Free For Use"
echo "###############################################"
exit
