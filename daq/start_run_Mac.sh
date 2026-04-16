#!/bin/bash
# start_run_Mac.sh -- Mac2020 side of run start
# Called by DAQ start_run.sh via SSH after caput Start
#
# 1. SCP expName.sh from DAQ
# 2. GenElog.py -> elog.txt
# 3. GenElogExtra.py -> elog_extra.txt
# 4. Combine -> elogFull.txt
# 5. Post to elog
# 6. Post to Discord

export HELIOS_MAPPING=~/digios/analysis/working/GeneralSortMapping.h
SCRIPTDIR=$(dirname "$(realpath "$0")")
DAQ_HOST=helios@192.168.1.2

echo "[start_run_Mac] Starting..."

# 1. SCP expName.sh from DAQ
echo "[start_run_Mac] Fetching expName.sh from DAQ..."
scp ${DAQ_HOST}:~/digios/expName.sh ~/digios/expName.sh
source ~/digios/expName.sh

RUN=${LastRunNum}
runLen=${#RUN}
if [ ${runLen} -eq 1 ]; then RUN="00"${RUN}
elif [ ${runLen} -eq 2 ]; then RUN="0"${RUN}; fi

echo "[start_run_Mac] Experiment: ${expName} | RUN: ${RUN}"

# Elog name conversion (h096_31Si_dp -> H096_31Si_dp)
if [ "${expName}" == "ARR01" ]; then
    elogName="ARR01"
else
    elogName="H"${expName:1}
fi

# 2. GenElog.py
echo "[start_run_Mac] Generating elog entry..."
python3 ${SCRIPTDIR}/GenElog.py start

# 3. GenElogExtra.py
echo "[start_run_Mac] Generating detector status table..."
python3 ${SCRIPTDIR}/GenElogExtra.py > ~/elog_extra.txt 2>/dev/null

# 4. Combine
cat ~/elog.txt ~/elog_extra.txt > ~/elogFull.txt
echo "[start_run_Mac] Combined elog: $(wc -c < ~/elogFull.txt) bytes"

# 5. Post to elog
echo "[start_run_Mac] Posting to elog..."
ELOG_BIN=~/elog_CloudFlare/BUILD/elog
if [ ! -f "${ELOG_BIN}" ]; then
    ELOG_BIN=~/bin/elog
fi

ELOG_OUT=$(${ELOG_BIN} -s -p 443 -h elog.phy.anl.gov -l ${elogName} \
    -u MasterHelios helios \
    -a Category=Runs -a Subject="RUN-${RUN} started" \
    -n 2 -m ~/elogFull.txt 2>&1)

# Parse elog ID
ELOG_ID=$(echo "${ELOG_OUT}" | awk -F'ID=' '{print $2}' | tr -d '[:space:]')
echo "ID=${ELOG_ID}" > ~/elogID.txt
echo "[start_run_Mac] Elog posted, ID=${ELOG_ID}"

# 6. Push to Discord
echo "[start_run_Mac] Pushing to Discord..."
${SCRIPTDIR}/push2Discord.sh ${elogName} 1 2>/dev/null

echo "[start_run_Mac] Done."
