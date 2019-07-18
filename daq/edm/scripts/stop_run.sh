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

currentDate=$(date)

echo "         stop at ${currentDate}"
echo "         stop at ${currentDate}" >> ${daqDataPath}/${expName}/data/RunTimeStamp.dat

#download the elog entry
elogContent=~/digios/analysis/working/elog_content.txt
elog -h www.phy.anl.gov -d elog -p 443 -l ${expName^^} -s -u GeneralHelios helios -w ${elogID} > ${elogContent}
cutLineNum=$(grep -n "==============" elog_content.txt | cut -d: -f1)
cutLineNum=$((cutLineNum+1))
tail -n +${cutLineNum} ${elogContent} > ${elogContent} 

echo "         stop at ${currentDate}" >> ~/digios/analysis/working/elog.txt

# this will create seperated elog entry 
~/digios/daq/edm/scripts/elog.sh stop
# in order to replace the elog entry, comment out above line, the GrafanaElog.sh in heliosdb (you have to edit GrafanaElog.sh)  will do the job

caput Online_CS_StartStop Stop
caput Online_CS_SaveData "No Save"

curl -s -XPOST "http://heliosDB:8086/write?db=testing" --data-binary "SavingData,expName=${expName} value=0" --max-time 1 --connect-timeout 1

echo "==== wait for 5 sec"
sleep 5

ssh heliosdatabase@heliosdb '/home/heliosdatabase/GrafanaElog.sh'

echo wait 10 seconds before closing the IOCs
sleep 10

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

echo "=== wait 10 seconds before next run ==="
sleep 10

echo -e "------------ The Run\033[0;31m${RUN}\033[0m has now been STOPPED  ----------------"
