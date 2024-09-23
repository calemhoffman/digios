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

if [ $# -eq 0 ];then
    echo 'Ctrl+C to cancel with no harm.'
    read -p 'Singleline comment for this run: ' COMMENT
else
    COMMENT=$1
fi

currentDate=$(date)

echo "         stop at ${currentDate}"
echo "         stop at ${currentDate}| ${COMMENT}" >> ${daqDataPath}/${expName}/data/RunTimeStamp.dat

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

du -hc ${HELIOSSYS}/analysis/data/${expName}_run_${RUN}*

totalFileSize=$(du -hc ${HELIOSSYS}/analysis/data/${expName}_run_${RUN}* | tail -n1 | awk {'print $1'})

echo "==== wait for 2 sec"
sleep 2

echo "take screenshot and copy from mac2017"
screenShot=${HELIOSSYS}/analysis/working/grafanaElog.jpg
ssh heliosdigios@${dataBaseAddress} '/Users/heliosdigios/digios/daq/GrafanaWeb.sh' #this is in mac2017
scp heliosdigios@${dataBaseAddress}:~/grafanaElog.jpg ${screenShot}
# send the screenshot to Mac2020 for elog
scp ${screenShot} heliosdigios@${mac2020IP}:~/grafanaElog.jpg

elogContext=${HELIOSSYS}/analysis/working/elogEndRun.txt
echo "         stop at ${currentDate} <br />" > ${elogContext}
echo "grafana screenshot is attached. <br />" >> ${elogContext}
echo " total File Size = ${totalFileSize} <br /> " >> ${elogContext}
echo "-----------------------------------------------</p>" >> ${elogContext}
echo "$COMMENT <br />" >> ${elogContext}

scp ${elogContext} heliosdigios@${mac2020IP}:~/.

# tell Mac2020 run push2elog script
if [ $expName = "ARR01" ]; then
  elogName=$expName
elif [  ${expName} == "h087_Tritium" ]; then
  elogName="H087_Tritium"
else
  elogName="H"${expName:1}
fi
ssh heliosdigios@${mac2020IP} "/Users/heliosdigios/digios/daq/push2Elog.sh stop ${expName} ${RUN}"


echo ">>>>>>>>>>>>>>> push to Discord"

#source ~/Slack_Elog_Notification.sh
source ~/Discord_webhooks.sh
slackMsg="https://www.phy.anl.gov/elog/${elogName}/${elogID}\n"
auxMsg="stop at ${currentDate} \ntotal File Size = ${totalFileSize}\n$COMMENT"

#curl -X POST -H 'Content-type: application/json' --data '{"text":"'"${slackMsg}${auxMsg}"'"}' ${WebHook}
curl -H "Content-Type: application/json" -X POST -d "{\"content\":\"${slackMsg}${auxMsg}\"}" $WEBHOOK_DAQ_URL


echo wait 2 seconds before closing the IOCs
sleep 2

#number of IOCS/Rec. in use
LIMIT=4

for ((a=1; a <= LIMIT ; a++))  # Double parentheses, and naked "LIMIT"
do 
   \rm -rf temp
   ps aux | grep ioc$a | grep 'xterm' >temp
   while read -r var1 var2 var3   
   do       
     kill -9 $var2      
   done <temp
done        
rm -rf temp

echo -e "\033[1;31m ### Globus is disabled ###\033[m"
#echo "=== wait 5 seconds before submit a transfer to LCRC ==="
#sleep 5
#${HELIOSSYS}/daq/edm/scripts/globus_out.py
#${HELIOSSYS}/daq/edm/scripts/globus_in.py

#===== Get root_data/ from LCRC to MAC
#MACEndPoint=0910df94-fb59-11e9-9945-0a8c187e8c12
#LCRCEndPoint=57b72e31-9f22-11e8-96e1-0a6d4e044368

#LCRCPath=/lcrc/project/HELIOS/digios/analysis/root_data/
#MACPath=/Users/heliosdigios/digios/analysis/root_data/
#globus transfer -r -s checksum  $LCRCEndPoint:$LCRCPath  $MACEndPoint:$MACPath


echo -e "------------ The Run\033[0;31m${RUN}\033[0m has now been STOPPED  ----------------"

echo "this window close in 5 sec."
sleep 5
