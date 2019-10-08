#!/bin/bash -l

source ~/digios/expName.sh

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

curl -s -XPOST "http://heliosDB:8086/write?db=testing" --data-binary "SavingData,expName=${expName} value=0" --max-time 1 --connect-timeout 1

echo "==== wait for 2 sec"
sleep 2

# take screenshot and copy from heliosDB
screenShot=~/digios/analysis/working/grafanaElog.jpg
ssh heliosdatabase@heliosdb '/home/heliosdatabase/digios/daq/GrafanaElog.sh'
scp heliosdatabase@heliosdb:~/grafanaElog.jpg ${screenShot}

if [ -z ${elogID} ]; then
    # this will create seperated elog entry
    echo "         stop at ${currentDate}" >> ~/digios/analysis/working/elog.txt
    elog -h websrv1.phy.anl.gov -p 8080 -l "H"${expName:1} -u GeneralHelios helios -a Category=Run -a RunNo=${LastRunNum} -a Subject="Stop Run ${LastRunNum}" -n 1 -m ~/digios/analysis/working/elog.txt -f ${screenShot}

else
    # in order to replace the elog entry, comment out above line, the GrafanaElog.sh in heliosdb (you have to edit GrafanaElog.sh)  will do the job
    echo "---- downloading the elog entry elohID=${elogID}"
    elogContext=~/digios/analysis/working/elog.txt
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
    echo "-----------------------------------------------</p>" >> ${elogContext}
    echo "$COMMENT <br />" >> ${elogContext}
    elog -h websrv1.phy.anl.gov -p 8080 -l ${elogName} -u GeneralHelios helios -e ${elogID} -n ${encodingID} -m ${elogContext} -f ${screenShot}

    slackMsg="elogID=${elogID} is updated.  https://www.phy.anl.gov/elog/${elogName}/${elogID}\n"
    auxMsg="stop at ${currentDate} \n$COMMENT"
    curl -X POST -H 'Content-type: application/json' --data '{"text":"'"${slackMsg}${auxMsg}"'"}' https://hooks.slack.com/services/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

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

echo "=== wait 5 seconds before next run ==="
sleep 5

echo -e "------------ The Run\033[0;31m${RUN}\033[0m has now been STOPPED  ----------------"
