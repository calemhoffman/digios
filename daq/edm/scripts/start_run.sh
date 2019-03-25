#!/bin/bash -l
read -p 'Enter the Run Number (e.g. 001): ' RUN
read -p 'Singleline comment for this run: ' COMMENT

#check RUN is 3 digit
runLen=${#RUN}
if [ ${runLen} -eq 1 ]; then
   RUN="00"${RUN}
elif [ ${runLen} -eq 2 ]; then
   RUN="0"${RUN}
fi;

set DIGIOSRUNNUM
DIGIOSRUNNUM=${RUN}
export DIGIOSRUNNUM

source ~/digios/expName.sh #load expName

#echo "RUN-${RUN} start at $(date), $COMMENT"
echo "RUN-${RUN} start at $(date), $COMMENT" >> ~/digios/analysis/working/RunTimeStamp.txt 

#Start run and save first!?!?
caput Online_CS_SaveData Save
caput Online_CS_StartStop Start

export TERM=vt100
echo " terminals" 
# Now spawn receiver windows
xterm -T ioc1 -geometry 110x24+0+0  -sb  -sl 1000 -e "gtReceiver4" "ioc1" "${expName}_run_$DIGIOSRUNNUM.gtd01" "2000000000" "14" &

xterm -T ioc2 -geometry 110x24+0+250  -sb  -sl 1000 -e "gtReceiver4" "ioc2" "${expName}_run_$DIGIOSRUNNUM.gtd02" "2000000000" "14" &

xterm -T ioc3 -geometry 110x24+0+500  -sb  -sl 1000 -e "gtReceiver4" "ioc3" "${expName}_run_$DIGIOSRUNNUM.gtd03" "2000000000" "14" &

xterm -T ioc4 -geometry 110x24+0+750  -sb  -sl 1000 -e "gtReceiver4" "ioc4" "${expName}_run_$DIGIOSRUNNUM.gtd04" "2000000000" "14" &

sleep 2
echo Run Started...
