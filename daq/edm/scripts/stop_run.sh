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

echo -e "------------ Stopping the current Run\033[0;31m${RUN}\033[0m ------------------"

echo "         stop at $(date)" >> ~/digios/analysis/working/RunTimeStamp.dat

caput Online_CS_StartStop Stop
caput Online_CS_SaveData "No Save"

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
echo -e "------------ The Run\033[0;31m${RUN}\033[0m has now been STOPPED  ----------------"
