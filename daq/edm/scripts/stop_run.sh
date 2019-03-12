#!/bin/bash -l

echo ------------ Stopping the current run ------------------

expname=iss000
echo "         stop at $(date)" >> ~/experiments/${expname}/RunTimeStamp.txt

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
echo ------------ The run has now been STOPPED  ----------------