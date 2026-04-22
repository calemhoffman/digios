#!/bin/bash

elogName=$1
isStartRun=$2

source ~/Discord_webhooks.sh
source ~/elogID.txt
elogID=${ID}

# Get run number from expName.sh
source ~/digios/expName.sh 2>/dev/null
runNum=$((LastRunNum))

# Convert HTML to Discord Markdown
html_to_markdown() {
    echo "$1" | \
        sed 's/<b>/**/g' | \
        sed 's/<\/b>/**/g' | \
        sed 's/<br\/>//g' | \
        sed 's/<br \/>//g' | \
        sed 's/<[^>]*>//g' | \
        sed 's/"/\\"/g'
}

if [ $isStartRun -eq 1 ]; then
    elogLink="https://www.phy.anl.gov/elog/${elogName}/${elogID}"
    elogMsg=$(cat ~/elog.txt)
    elogMsg=$(html_to_markdown "$elogMsg" | sed 's/$/\\n/' | tr -d '\n' | sed 's/\\n$//')
    curl -H "Content-Type: application/json" -X POST -d "{\"content\":\"${elogLink}\n${elogMsg}\"}" $WEBHOOK_DAQ_URL
else
    elogMsg=$(cat ~/endRunElog.txt 2>/dev/null)
    elogMsg=$(html_to_markdown "$elogMsg" | sed 's/$/\\n/' | tr -d '\n' | sed 's/\\n$//')
    curl -H "Content-Type: application/json" -X POST -d "{\"content\":\"${elogMsg}\"}" $WEBHOOK_DAQ_URL
fi
