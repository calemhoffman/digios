#!/bin/bash
# stop_run_Mac.sh -- Mac2020 side of run stop
# Called by DAQ stop_run.sh via SSH after caput Stop
#
# 1. SCP expName.sh from DAQ
# 2. Grafana screenshot + elog download (PARALLEL)
# 3. Write endRunElog.txt, append to elog
# 4. Re-upload elog + push to Discord (PARALLEL)
# 5. Signal DAQ: touch /tmp/elog_done

export HELIOS_MAPPING=~/digios/analysis/working/GeneralSortMapping.h
SCRIPTDIR=$(dirname "$(realpath "$0")")
DAQ_HOST=helios@192.168.1.2

echo "[stop_run_Mac] Starting..."

# 1. SCP expName.sh from DAQ
echo "[stop_run_Mac] Fetching expName.sh from DAQ..."
scp ${DAQ_HOST}:~/digios/expName.sh ~/digios/expName.sh
source ~/digios/expName.sh

RUN=${LastRunNum}
runLen=${#RUN}
if [ ${runLen} -eq 1 ]; then RUN="00"${RUN}
elif [ ${runLen} -eq 2 ]; then RUN="0"${RUN}; fi

# Elog name conversion
if [ "${expName}" == "ARR01" ]; then
    elogName="ARR01"
else
    elogName="H"${expName:1}
fi

echo "[stop_run_Mac] Experiment: ${expName} | RUN: ${RUN}"

# Elog binary
ELOG_BIN=~/elog_CloudFlare/BUILD/elog
if [ ! -f "${ELOG_BIN}" ]; then
    ELOG_BIN=~/bin/elog
fi

source ~/elogID.txt 2>/dev/null

# 2. Grafana screenshot + elog download (PARALLEL)
echo "[stop_run_Mac] Starting parallel: Grafana screenshot + elog download..."

# 2a. Grafana screenshot (background)
source ~/digios/daq/edm/scripts/DataBaseAddress.sh
(
    ssh heliosdigios@${dataBaseAddress} '/Users/heliosdigios/digios/daq/GrafanaWeb.sh' 2>/dev/null
    scp heliosdigios@${dataBaseAddress}:~/grafanaElog.jpg ~/grafanaElog.jpg 2>/dev/null
    echo "[stop_run_Mac] Grafana screenshot done."
) &
PID_GRAFANA=$!

# 2b. Download live elog entry (background)
(
    if [ -n "${ID}" ]; then
        ${ELOG_BIN} -s -p 443 -h elog.phy.anl.gov -l ${elogName} \
            -u MasterHelios helios \
            -w ${ID} > ~/elog_live.txt 2>/dev/null
        # Strip metadata header from elog -w output (body only for -m upload)
        sed -i '' '1,/^========/d' ~/elog_live.txt
        echo "[stop_run_Mac] Elog downloaded: $(wc -c < ~/elog_live.txt) bytes"
    else
        echo "[stop_run_Mac] WARNING: no start elog ID found"
        echo "" > ~/elog_live.txt
    fi
) &
PID_ELOG_DL=$!

# Wait for both
wait $PID_GRAFANA $PID_ELOG_DL
echo "[stop_run_Mac] Parallel step done."

# 3. Write endRunElog.txt and append to elog
currentDate=$(date)
STOP_COMMENT="${lastStopComment:-stopped}"

cat > ~/endRunElog.txt << EOFSTOP
<br/>
<b>RUN-${RUN}</b> stopped at ${currentDate}.<br/>
<b>Total file size:</b> ${totalFileSize:-unknown}<br/>
<b>Stop reason:</b> ${STOP_COMMENT}<br/>
-----------------------------------------------<br/>
EOFSTOP

cat ~/endRunElog.txt >> ~/elog_live.txt
echo "[stop_run_Mac] Wrote endRunElog.txt, appended to elog."

# 4. Re-upload elog + push to Discord (PARALLEL)
echo "[stop_run_Mac] Starting parallel: elog upload + Discord..."

# 4a. Re-upload elog (background)
(
    if [ -n "${ID}" ]; then
        if [ -f ~/grafanaElog.jpg ]; then
            ${ELOG_BIN} -s -p 443 -h elog.phy.anl.gov -l ${elogName} \
                -u MasterHelios helios \
                -e ${ID} \
                -n 2 -m ~/elog_live.txt \
                -f ~/grafanaElog.jpg 2>&1
        else
            ${ELOG_BIN} -s -p 443 -h elog.phy.anl.gov -l ${elogName} \
                -u MasterHelios helios \
                -e ${ID} \
                -n 2 -m ~/elog_live.txt 2>&1
        fi
        echo "[stop_run_Mac] Elog entry ${ID} updated."
    else
        ${ELOG_BIN} -s -p 443 -h elog.phy.anl.gov -l ${elogName} \
            -u MasterHelios helios \
            -a Category=Runs -a Subject="RUN-${RUN} stopped" \
            -n 2 -m ~/elog_live.txt 2>&1
        echo "[stop_run_Mac] Elog posted as new entry."
    fi
) &
PID_ELOG_UP=$!

# 4b. Push to Discord (background)
(
    ${SCRIPTDIR}/push2Discord.sh ${elogName} 0 2>/dev/null
    echo "[stop_run_Mac] Discord posted."
) &
PID_DISCORD=$!

# Wait for both
wait $PID_ELOG_UP $PID_DISCORD
echo "[stop_run_Mac] Parallel step done."

# 5. Signal DAQ: elog + Discord complete, safe to start next run
ssh ${DAQ_HOST} "touch /tmp/elog_done" 2>/dev/null

echo "[stop_run_Mac] Done. Signaled DAQ."
