#!/bin/bash

flag=$1
elogName=$2
RunNo=$3


elogTxt=/Users/heliosdigios/elog.txt
elogFullTxt=/Users/heliosdigios/elogFull.txt
elogIDTxt=/Users/heliosdigios/elogID.txt

grafanaElog=/Users/heliosdigios/grafanaElog.jpg
elogEndTxt=/Users/heliosdigios/elogEndRun.txt

if [ ${flag} == "start" ]; then
   echo "============ push to elog at start run"
   echo "elogName = ${elogName}, RunNo = ${RunNo}"

   # Use elogFull.txt for elog if it exists (has detector table), otherwise elog.txt
   if [ -f ${elogFullTxt} ]; then
      elogPost=${elogFullTxt}
      echo "Using elogFull.txt (with detector table)"
   else
      elogPost=${elogTxt}
   fi

   IDStr=$(~/bin/elog -s -p 443 -h elog.phy.anl.gov -l ${elogName} -u GeneralHelios helios -a Category=Run -a RunNo=${RunNo} -a Subject="Start Run ${RunNo}" -n 2 -m ${elogPost})

   echo "-----"
   echo ${IDStr} 

   IDStr=$(echo ${IDStr} | grep "transmitted" | awk -F'ID=' '{print $2}')

   echo ${IDStr}
   echo "ID=${IDStr}" > ${elogIDTxt}

   scp ${elogIDTxt} helios@192.168.1.2:~/.
   echo "sent ${elogIDTxt} back to DAQ"

fi


if [ ${flag} == "stop" ]; then
   echo "============ push to elog at stop run"
   echo "elogName = ${elogName}, RunNo = ${RunNo}"
 
# the following code is for editing elog entry, but the new elog server fail to do so.  
   source ${elogIDTxt}
   echo "elogID = "${ID}

   ~/bin/elog -s -p 443 -h elog.phy.anl.gov -l ${elogName} -u GeneralHelios helios -w ${ID} > ${elogTxt}

   cutLineNum=$(grep -n "==============" ${elogTxt} | cut -b 1,2)
   #echo "cut Line Number : "${cutLineNum}
   encoding=$(grep "Encoding" ${elogTxt} | awk '{print $2}')
   if [ $encoding = "plain" ]; then encodingID=1 ; fi
   if [ $encoding = "HTML" ]; then encodingID=2 ; fi
   if [ $encoding = "ELcode" ]; then encodingID=0 ; fi
   echo "===== "${encoding}

   #remove all header
   sed -i '' -e "1,${cutLineNum}d" ${elogTxt}
   #append elogEnnRun.txt
   cat ${elogEndTxt} >> ${elogTxt}

   ~/bin/elog -s -p 443 -h elog.phy.anl.gov -l ${elogName} -u GeneralHelios helios -e ${ID} -n ${encodingID} -m ${elogTxt} -f ${grafanaElog}

#  elog -s -p 443 -h elog.phy.anl.gov -l ${elogName} -u GeneralHelios helios -a Category=Run -a RunNo=${RunNo} -a Subject="Stop Run ${RunNo}" -n 2 -m ${elogEndTxt} -f ${grafanaElog}

fi
