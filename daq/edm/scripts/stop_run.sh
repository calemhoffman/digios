#!/bin/bash -l

echo ------------ Stopping run --------------------------

caput Online_CS_StartStop Stop
caput Online_CS_SaveData "No Save"

echo wait 5 seconds before closing IOCs
sleep 5

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
echo ------------ finished with run stop ----------------