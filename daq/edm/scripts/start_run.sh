#!/bin/bash -l

constFile=~/digios/expName.sh
source ${constFile} #load expName

#==== next experiment number
LastRunNum=$((LastRunNum + 1))

RUN=${LastRunNum}
#check RUN is 3 digit
runLen=${#RUN}
if [ ${runLen} -eq 1 ]; then
   RUN="00"${RUN}
elif [ ${runLen} -eq 2 ]; then
   RUN="0"${RUN}
fi;

echo ""
echo "=========================================="
echo "Starting run : ${RUN}"
echo "=========================================="

#==== read comment
echo 'Ctrl+C to cancel with no harm.'
read -p 'Singleline comment for this run: ' COMMENT

echo "#!/bin/bash -l" > ${constFile}
echo "expName=${expName}" >> ${constFile}
echo "daqDataPath=${daqDataPath}" >> ${constFile}
echo "LastRunNum=${LastRunNum}" >> ${constFile}

set DIGIOSRUNNUM
DIGIOSRUNNUM=${RUN}
export DIGIOSRUNNUM

currentDate=$(date)

echo "RUN-${RUN} start at ${currentDate}, $COMMENT"
echo "RUN-${RUN} start at ${currentDate}, $COMMENT" >> ${daqDataPath}/${expName}/data/RunTimeStamp.dat
echo "RUN-${RUN} start at ${currentDate}, $COMMENT" > ~/digios/analysis/working/elog.txt
trigger=$(caget GLBL:DIG:trigger_mux_select | awk '{print$2}')
echo "trigger : " $trigger >> ~/digios/analysis/working/elog.txt

#Start run and save first!?!?
caput Online_CS_SaveData Save
caput Online_CS_StartStop Start


~/digios/daq/edm/scripts/elog.sh start

curl -s -XPOST "http://heliosDB:8086/write?db=testing" --data-binary "SavingData,expName=${expName} value=1" --max-time 1 --connect-timeout 1

export TERM=vt100
echo " terminals" 
#Now spawn receiver windows
xterm -T ioc1 -geometry 110x24+0+0  -sb  -sl 1000 -e "gtReceiver4" "ioc1" "${expName}_run_$DIGIOSRUNNUM.gtd01" "2000000000" "14" &

xterm -T ioc2 -geometry 110x24+0+250  -sb  -sl 1000 -e "gtReceiver4" "ioc2" "${expName}_run_$DIGIOSRUNNUM.gtd02" "2000000000" "14" &

xterm -T ioc3 -geometry 110x24+0+500  -sb  -sl 1000 -e "gtReceiver4" "ioc3" "${expName}_run_$DIGIOSRUNNUM.gtd03" "2000000000" "14" &

xterm -T ioc4 -geometry 110x24+0+750  -sb  -sl 1000 -e "gtReceiver4" "ioc4" "${expName}_run_$DIGIOSRUNNUM.gtd04" "2000000000" "14" &

sleep 2
echo Run${RUN} Started...
