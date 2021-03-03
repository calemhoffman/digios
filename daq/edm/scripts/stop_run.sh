#!/bin/bash -l

source ${HELIOSSYS}/expName.sh

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

curl -s -XPOST "http://mac2017:8086/write?db=testing" --data-binary "SavingData,expName=${expName} value=0" --max-time 1 --connect-timeout 1

du -hc ${HELIOSSYS}/analysis/data/${expName}_run_${RUN}*

totalFileSize=$(du -hc ${HELIOSSYS}/analysis/data/${expName}_run_${RUN}* | tail -n1 | awk {'print $1'})

echo "==== wait for 2 sec"
sleep 2

# take screenshot and copy from heliosDB
#screenShot=${HELIOSSYS}/analysis/working/grafanaElog.jpg
#ssh heliosdatabase@heliosdb '/home/heliosdatabase/digios/daq/GrafanaElog.sh'
#scp heliosdatabase@heliosdb:~/grafanaElog.jpg ${screenShot}

# take screenshot and copy from mac2017
screenShot=${HELIOSSYS}/analysis/working/grafanaElog.jpg
ssh heliosdigios@mac2017 '/home/heliosdatabase/digios/daq/GrafanaElog.sh'
scp heliosdigios@mac2017:~/grafanaElog.jpg ${screenShot}

if [ -z ${elogID} ]; then
    # this will create seperated elog entry
    echo "         stop at ${currentDate}" >> ${HELIOSSYS}/analysis/working/elog.txt
    elog -h websrv1.phy.anl.gov -p 8080 -l "H"${expName:1} -u GeneralHelios helios -a Category=Run -a RunNo=${LastRunNum} -a Subject="Stop Run ${LastRunNum}" -n 1 -m ${HELIOSSYS}/analysis/working/elog.txt -f ${screenShot}

else
    # in order to replace the elog entry, comment out above line, the GrafanaElog.sh in heliosdb (you have to edit GrafanaElog.sh)  will do the job
    echo "---- downloading the elog entry elohID=${elogID}"
    elogContext=${HELIOSSYS}/analysis/working/elog.txt
    #elog -h www.phy.anl.gov -d elog -p 443 -l "H"${expName:1} -s -u GeneralHelios helios -w ${elogID} > ${elogContext}
    if [ $expName = "ARR01" ]; then
	elogName=$expName
    else
	elogName="H"${expName:1}
    fi

    elog -h websrv1.phy.anl.gov -p 8080 -l ${elogName} -u GeneralHelios helios -w ${elogID} > ${elogContext}
    cutLineNum=$(grep -n "==============" ${elogContext} | cut -d: -f1)
    #check encoding
    encoding=$(grep "Encoding" ${elogContext} | awk '{print $2}')
    if [ $encoding = "plain" ]; then encodingID=1 ; fi
    if [ $encoding = "HTML" ]; then encodingID=2 ; fi
    if [ $encoding = "ELcode" ]; then encodingID=0 ; fi
    #remove all header
    sed -i "1,${cutLineNum}d" ${elogContext}
    #fill stop time
    echo "         stop at ${currentDate} <br />" >> ${elogContext}
    echo "grafana screenshot is attached. <br />" >> ${elogContext}
    echo " total File Size = ${totalFileSize} <br /> " >> ${elogContext}
    echo "-----------------------------------------------</p>" >> ${elogContext}
    echo "$COMMENT <br />" >> ${elogContext}
    elog -h websrv1.phy.anl.gov -p 8080 -l ${elogName} -u GeneralHelios helios -e ${elogID} -n ${encodingID} -m ${elogContext} -f ${screenShot}

    source ~/Slack_Elog_Notification.sh
    slackMsg="https://www.phy.anl.gov/elog/${elogName}/${elogID}\n"
    auxMsg="stop at ${currentDate} \ntotal File Size = ${totalFileSize}\n$COMMENT"
    curl -X POST -H 'Content-type: application/json' --data '{"text":"'"${slackMsg}${auxMsg}"'"}' ${WebHook}

fi

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
#     echo $a
     kill -2 $var2      
   done <temp
done        
rm -rf temp

echo "=== wait 5 seconds before submit a transfer to LCRC ==="
sleep 5

${HELIOSSYS}/daq/edm/scripts/globus_out.py
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
