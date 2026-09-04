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
# Prefer the ryan_ANL build (ttang/elog-ryan branch ANL): libcurl transport
# plus HTTP/2-aware "location:" header parsing so the ID is reliably returned.
# Fall back to the older dhp/elog:ANL build, then a plain ~/bin/elog.
ELOG_BIN=~/elog-ryan-build/elog
if [ ! -x "${ELOG_BIN}" ]; then
    ELOG_BIN=~/elog_CloudFlare/BUILD/elog
fi
if [ ! -x "${ELOG_BIN}" ]; then
    ELOG_BIN=~/bin/elog
fi

# Clear any stale ID from a prior run so a failure here can't cause
# stop_run_Mac.sh to edit the previous run's entry.
rm -f ~/elogID.txt

ELOG_ERR=$(mktemp)
ELOG_OUT=$(${ELOG_BIN} -s -p 443 -h elog.phy.anl.gov -l ${elogName} \
    -u MasterHelios helios \
    -a Category=Runs -a Subject="RUN-${RUN} started" \
    -n 2 -m ~/elogFull.txt 2>"${ELOG_ERR}")
ELOG_RC=$?

# Parse elog ID: only accept an ID printed on elog's success line, so a
# CloudFlare/WAF page containing "Ray ID=..." can't be mistaken for success.
ELOG_ID=$(printf '%s\n' "${ELOG_OUT}" \
    | grep 'successfully transmitted' \
    | grep -oE 'ID=[0-9]+' | tail -1 | cut -d= -f2)

if [[ "${ELOG_ID}" =~ ^[0-9]+$ ]]; then
    echo "ID=${ELOG_ID}" > ~/elogID.txt
    echo "[start_run_Mac] Elog posted, ID=${ELOG_ID}"
else
    echo "[start_run_Mac] ERROR: failed to parse elog ID (rc=${ELOG_RC})" >&2
    echo "[start_run_Mac] --- elog stdout ---" >&2
    printf '%s\n' "${ELOG_OUT}" >&2
    echo "[start_run_Mac] --- elog stderr ---" >&2
    cat "${ELOG_ERR}" >&2
    rm -f "${ELOG_ERR}"
    exit 1
fi
rm -f "${ELOG_ERR}"

# 6. Push to Discord
echo "[start_run_Mac] Pushing to Discord..."
${SCRIPTDIR}/push2Discord.sh ${elogName} 1 2>/dev/null

echo "[start_run_Mac] Done."
