elogName=$1
isStartRun=$2

source ~/Discord_webhooks.sh
source ~/elogID.txt
elogID=${ID}

if [ $isStartRun -eq 1 ]; then
    slackMsg="**RUN=${LastRunNum}**.  https://www.phy.anl.gov/elog/${elogName}/${elogID}\n"
    elogMsg=`cat ~/elog.txt`
    elogMsg=$(echo "$elogMsg" | sed 's/\*/=/g' | sed -e 's/<br \/>//g' | sed 's/"/\\"/g; s/$/\\n/' | tr -d '\n' | sed 's/\\n$//')
    curl -H "Content-Type: application/json" -X POST -d "{\"content\":\"${slackMsg}${elogMsg}\"}" $WEBHOOK_DAQ_URL

else

    slackMsg="https://www.phy.anl.gov/elog/${elogName}/${elogID}\n"
    #auxMsg="stop at ${currentDate} \ntotal File Size = ${totalFileSize}\n$COMMENT"
    auxMsg=`cat ~/.endRunMsg.txt`
    curl -H "Content-Type: application/json" -X POST -d "{\"content\":\"${slackMsg}${auxMsg}\"}" $WEBHOOK_DAQ_URL

fi

#msg="hahaha"
#curl -H "Content-Type: application/json" -X POST -d "{\"content\":\"${msg}\"}" $WEBHOOK_DAQ_URL
