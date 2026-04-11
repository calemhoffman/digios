#!/bin/bash -l

source ${HELIOSSYS}/expName.sh

source ${HELIOSSYS}/daq/edm/scripts/DataBaseAddress.sh

RUN=${LastRunNum}

#check RUN is 3 digit
runLen=${#RUN}
if [ ${runLen} -eq 1 ]; then
   RUN="00"${RUN}
elif [ ${runLen} -eq 2 ]; then
   RUN="0"${RUN}
fi;

echo -e "\n------------ Stopping the current Run\033[0;31m${RUN}\033[0m ------------------"

# Usage:
#   Human (EDM/manual): stop_run.sh "comment"   or   stop_run.sh  (interactive)
#   HELIOS AI:          stop_run.sh --ai "one-line comment"
AI_MODE=false
if [ "$1" == "--ai" ]; then
    AI_MODE=true
    COMMENT=$2
    echo "[AI MODE] stopped by HELIOS AI"
elif [ $# -eq 0 ];then
    echo 'Ctrl+C to cancel with no harm.'
    read -p 'Singleline comment for this run: ' COMMENT
else
    COMMENT=$1
fi

currentDate=$(date)

echo "         stop at ${currentDate}"
echo "         stop at ${currentDate}| ${COMMENT}" >> ${daqDataPath}/${expName}/RunTimeStamp.dat

caput Online_CS_StartStop Stop
caput Online_CS_SaveData "No Save"

if [ -z  "$COMMENT" ]; then
    comment2="nan"
else
    comment2="${COMMENT// /\ }"
    comment2="${comment2//,/\,}"
fi
echo $comment2
curl -s -XPOST "http://${dataBaseAddress}:8086/write?db=testing" --data-binary "SavingData,expName=${expName},comment=Stop_RUN-${comment2} value=0" --max-time 1 --connect-timeout 1

du -hc ${daqDataPath}/${expName}/${expName}_run_${RUN}*

totalFileSize=$(du -hc ${daqDataPath}/${expName}/${expName}_run_${RUN}* | tail -n1 | awk {'print $1'})

# Update expName.sh with stop comment
echo "#!/bin/bash -l" > ${HELIOSSYS}/expName.sh
echo "expName=${expName}" >> ${HELIOSSYS}/expName.sh
echo "daqDataPath=${daqDataPath}" >> ${HELIOSSYS}/expName.sh
echo "LastRunNum=${LastRunNum}" >> ${HELIOSSYS}/expName.sh
echo "lastRunComment=\"${COMMENT}\"" >> ${HELIOSSYS}/expName.sh
echo "lastStopComment=\"${COMMENT}\"" >> ${HELIOSSYS}/expName.sh
echo "totalFileSize=\"${totalFileSize}\"" >> ${HELIOSSYS}/expName.sh

echo "==== wait for 2 sec"
sleep 2

#==== Mac2020 handles all elog + Discord + Grafana screenshot
echo "============= Calling Mac2020 for elog + Discord"
ssh heliosdigios@${mac2020IP} "/Users/heliosdigios/digios/daq/stop_run_Mac.sh" &

echo "============= Calling Pi for run status"
ssh ryan@192.168.1.100 "python3 ~/.openclaw/workspace/skills/helios-status/gen_run_status.py" &

echo "wait 2 seconds before closing the IOCs"
sleep 2

#number of IOCS/Rec. in use
LIMIT=4

for ((a=1; a <= LIMIT ; a++))
do
   \rm -rf temp
   ps aux | grep ioc$a | grep 'xterm' >temp
   while read -r var1 var2 var3
   do
     kill -9 $var2
   done <temp
done
rm -rf temp

echo -e "------------ The Run\033[0;31m${RUN}\033[0m has now been STOPPED  ----------------"

if [ "$AI_MODE" != "true" ]; then echo "this window close in 50 sec."
sleep 50; fi
