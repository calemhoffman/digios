#!/bin/bash -l
export TERM=vt100
explocation=/home/helios/experiments/iss000/daq
datalocation=/media/DIGIOSDATA3/data/iss000

cd ${datalocation}
xterm -T stopdata -fa "Monospace" -fs 12 -bg rgb:cd/20/2c -fg white -geometry 130x35+500+20 -e "${explocation}/edm/scripts/stop_run.sh"
echo Pause for a few seconds after stopped run.
sleep 2
echo Terminal is now Free For Use
exit
