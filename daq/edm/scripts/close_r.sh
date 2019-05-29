#!/bin/bash -l
source ~/digios/expName.sh #load expName
export TERM=vt100
explocation=/home/digios/daq
datalocation=/media/DIGIOSDATA3/${expName}/data

cd ${datalocation}
xterm -T stopdata -fa "Monospace" -fs 12 -bg rgb:cd/20/2c -fg white -geometry 130x35+500+20 -e "${explocation}/edm/scripts/stop_run.sh"
echo Pause for a few seconds after stopped run.
sleep 2
echo Terminal is now Free For Use
exit
