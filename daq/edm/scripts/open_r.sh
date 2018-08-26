#!/bin/bash -l

#if [ "${DIGIOSRUNNUM}" -eq "0" ]
#then
#echo DIGIOSRUNNUM of zero not accepted
#read -p 'Enter a Run Number: ' RUN
#export DIGIOSRUNNUM=${RUN}
#else
#export DIGIOSRUNNUM=$(expr ${DIGIOSRUNNUM} + 1)
#fi


export TERM=vt100
#cd /media/Digios\ Data/data/h057_he6
cd /media/DIGIOSDATA2/data/h060_pb208
xterm -T savedata -fa "Monospace" -fs 12 -bg rgb:cd/20/2c -fg white -geometry 130x35+500+20 -e "/global/devel/systems/helios/h060/edm/scripts/start_run.sh"
sleep 1


#echo Check for file opening
#xterm -T filecheck -geometry 130x35+500+20 -e "/global/devel/systems/helios/h057/edm/scripts/file_count.sh" "${DIGIOSRUNNUM}"
#sleep 1

cd /global/devel/systems/helios/h060
echo Run is underway
echo Done - Terminal Window is Free to use 
