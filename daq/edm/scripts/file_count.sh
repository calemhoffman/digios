#!/bin/bash -l

FILENUM=11 #number of files that should be opened
FILECOUNT=0
ATTEMPTS=0

function main() {
read -p 'Enter Run Number: ' RUNNUM
#RUNNUM=${DIGIOSRUNNUM}
}

function fileNum() {
( cd /media/DIGIOSDATA1/data/h057_he6/
find . -mindepth 1 -type f -name "*${RUNNUM}*" -exec printf x \; | wc -c
)
}

function fileLoop() {
while [ "${ATTEMPTS}" -lt "6" ] && [ "${FILECOUNT}" -le "${FILENUM}" ] ;
do

echo "Number of files for Run Number ${RUNNUM}"
local fNum=$(fileNum)
echo $fNum
FILECOUNT=$fNum
if [ "${FILECOUNT}" -lt "${FILENUM}" ]
then
echo wait some time and try, try again...
sleep 2
fi
ATTEMPTS=$(expr ${ATTEMPTS} + 1)
echo Attempt number ${ATTEMPTS}
done
}

finalCheck() {
if [ "${FILECOUNT}" -lt "${FILENUM}" ]
then
#stop_run.sh
echo FILES NEVER OPENED STOPPED AFTER ${ATTEMPTS} TRYS
else
echo Files are open, off we go!
fi
}

main
fileLoop
finalCheck
echo File Count Script Has Ended Will Close
sleep 20



