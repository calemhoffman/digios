#!/bin/bash -l
export TERM=vt100
cd /media/DIGIOSDATA2/data/h060_pb208
xterm -T savedata -geometry 130x35+500+20 stop_run.sh
echo "sleep for 1"
sleep 1
echo "Terminal is Free to Use"
exit
