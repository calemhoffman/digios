#!/bin/bash -l

constFile=${HELIOSSYS}/expName.sh
source ${constFile} #load expName, etc.

source ${HELIOSSYS}/daq/edm/scripts/DataBaseAddress.sh

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
if [ $# -eq 0 ];then
    echo 'Ctrl+C to cancel with no harm.'
    echo 'e.g. Target, beam rate, attenuation, slit, trigger, people on-shift, objective, etc... '
    read -p 'Singleline comment for this run: ' COMMENT
else
    COMMENT=$1
fi

if [ -z  "$COMMENT" ]; then  
    comment2="nan"
else
    comment2="${COMMENT// /\ }"
    comment2="${comment2//,/\,}"
fi
echo $comment2
curl -s -XPOST "http://${dataBaseAddress}:8086/write?db=testing" --data-binary "SavingData,expName=${expName},comment=Start_RUN-${comment2} value=1" --max-time 1 --connect-timeout 1


echo "#!/bin/bash -l" > ${constFile}
echo "expName=${expName}" >> ${constFile}
echo "daqDataPath=${daqDataPath}" >> ${constFile}
echo "LastRunNum=${LastRunNum}" >> ${constFile}

set DIGIOSRUNNUM
DIGIOSRUNNUM=${RUN}
export DIGIOSRUNNUM

currentDate=$(date)

echo "RUN-${RUN} start at ${currentDate}| $COMMENT"
echo "RUN-${RUN} start at ${currentDate}| $COMMENT" >> ${daqDataPath}/${expName}/data/RunTimeStamp.dat


elogFile=${HELIOSSYS}/analysis/working/elog.txt
echo "************************************************<br />" > ${elogFile}
echo "RUN-${RUN} start at ${currentDate}.<br />" >> ${elogFile}
echo "$COMMENT <br />" >> ${elogFile}
trigger=$(caget GLBL:DIG:trigger_mux_select | awk '{print$2}')
echo "trigger : $trigger <br />" >> ${elogFile}
haha=$(caget VME32:MTRG:SUM_X)
echo "${haha} <br />" >>${elogFile}
haha=$(caget VME32:MTRG:SUM_Y)
echo "${haha} <br />">>${elogFile}
echo "-----------------------------------------------<br />" >> ${elogFile}

#Start run and save first!?!?
caput Online_CS_SaveData Save
caput Online_CS_StartStop Start

echo "pushing to elog for started a run"

if [ ${expName} == "ARR01" ]; then
    elogName="ARR01"
else
    elogName="H"${expName:1}
fi

#### need to do this because the selog in DAQ is outdated.

scp ${elogFile} heliosdigios@${mac2020IP}:~/elog.txt
ssh heliosdigios@${mac2020IP} "/Users/heliosdigios/digios/daq/push2Elog.sh start ${elogName} ${RUN}"

echo "wait 3 sec for Mac2020 reply elog ID"
sleep 3

source ~/elogID.txt
echo ${ID}
elogIDStr="elogID="${ID}
echo ${elogID}

#source ~/Slack_Elog_Notification.sh
source ~/Discord_webhooks.sh
slackMsg="**RUN=${LastRunNum}**.  https://www.phy.anl.gov/elog/${elogName}/${elogID}\n"
elogMsg=`cat ${HELIOSSYS}/analysis/working/elog.txt`
#curl -X POST -H 'Content-type: application/json' --data '{"text":"'"${slackMsg}${elogMsg}"'"}' ${WebHook}

elogMsg=$(echo "$elogMsg" | sed 's/\*/=/g' | sed -e 's/<br \/>//g' | sed 's/"/\\"/g; s/$/\\n/' | tr -d '\n' | sed 's/\\n$//')
curl -H "Content-Type: application/json" -X POST -d "{\"content\":\"${slackMsg}${elogMsg}\"}" $WEBHOOK_DAQ_URL


echo "============== Opening IOC...."

export TERM=vt100
echo " terminals" 
#Now spawn receiver windows
xterm -T ioc1 -geometry 100x20+0+0  -sb  -sl 1000 -e "gtReceiver4" "ioc1" "${expName}_run_$DIGIOSRUNNUM.gtd01" "2000000000" "14" &

xterm -T ioc2 -geometry 100x20+0+300  -sb  -sl 1000 -e "gtReceiver4" "ioc2" "${expName}_run_$DIGIOSRUNNUM.gtd02" "2000000000" "14" &

xterm -T ioc3 -geometry 100x20+0+600  -sb  -sl 1000 -e "gtReceiver4" "ioc3" "${expName}_run_$DIGIOSRUNNUM.gtd03" "2000000000" "14" &

xterm -T ioc4 -geometry 100x20+0+900  -sb  -sl 1000 -e "gtReceiver4" "ioc4" "${expName}_run_$DIGIOSRUNNUM.gtd04" "2000000000" "14" &

${HELIOSSYS}/daq/edm/scripts/helios_database start

echo Run${RUN} Started...
echo "If Manual Run: This window will be closed after 30 sec"
sleep 30
