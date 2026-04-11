#!/bin/bash -l

constFile=${HELIOSSYS}/expName.sh
source ${constFile}

#==== safety reset
caput Online_CS_StartStop Stop
caput Online_CS_SaveData "No Save"

#==== clear stale elog signal
rm -f /tmp/elog_done

source ${HELIOSSYS}/daq/edm/scripts/DataBaseAddress.sh


#==== check HV is on (spot check u0, u200, u300)
HV_OK=false
for ch in u0 u200 u300; do
    HV_STATE=$(snmpget -v 2c -c guru 192.168.1.155 WIENER-CRATE-MIB::outputSwitch.$ch 2>/dev/null | grep -c "on(1)")
    if [ "$HV_STATE" -eq 1 ] 2>/dev/null; then
        HV_OK=true
        break
    fi
done
if [ "$HV_OK" == "false" ]; then
    if [ "$1" == "--ai" ]; then
        echo "[AI MODE] HV_NOT_ON"
        exit 1
    else
        echo -e "\033[1;33;40m"
        echo "  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
        echo "  !!!!    HV is NOT ON !!!!"
        echo "  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
        echo -e "\033[0m"
        echo "Run cancelled. No run number changed."
        sleep 30
        exit 1
    fi
fi

#==== next experiment number
LastRunNum=$((LastRunNum + 1))

RUN=${LastRunNum}
runLen=${#RUN}
if [ ${runLen} -eq 1 ]; then
   RUN="00"${RUN}
elif [ ${runLen} -eq 2 ]; then
   RUN="0"${RUN}
fi

echo ""
echo "=========================================="
echo "Starting run : ${RUN}"
echo "=========================================="

#==== read comment
AI_MODE=false
if [ "$1" == "--ai" ]; then
    AI_MODE=true
    COMMENT=$2
    echo "[AI MODE] started by HELIOS AI"
elif [ $# -eq 0 ]; then
    echo 'Ctrl+C to cancel with no harm.'
    echo 'e.g. Target, beam rate, attenuation, slit, trigger, people on-shift, objective, etc... '
    read -p 'Singleline comment for this run: ' COMMENT
else
    COMMENT=$1
fi

if [ -z "$COMMENT" ]; then
    comment2="nan"
else
    comment2="${COMMENT// /\ }"
    comment2="${comment2//,/\,}"
fi

curl -s -XPOST "http://${dataBaseAddress}:8086/write?db=testing" --data-binary "SavingData,expName=${expName},comment=Start_RUN-${comment2} value=1" --max-time 1 --connect-timeout 1

#==== update expName.sh with run number and comment
echo "#!/bin/bash -l" > ${constFile}
echo "expName=${expName}" >> ${constFile}
echo "daqDataPath=${daqDataPath}" >> ${constFile}
echo "LastRunNum=${LastRunNum}" >> ${constFile}
echo "lastRunComment=\"${COMMENT}\"" >> ${constFile}

set DIGIOSRUNNUM
DIGIOSRUNNUM=${RUN}
export DIGIOSRUNNUM

currentDate=$(date)

echo "RUN-${RUN} start at ${currentDate}| $COMMENT"
echo "RUN-${RUN} start at ${currentDate}| $COMMENT" >> ${daqDataPath}/${expName}/RunTimeStamp.dat

#Start run and save
caput Online_CS_SaveData Save
caput Online_CS_StartStop Start

#==== Mac2020 handles all elog + Discord
echo "============= Calling Mac2020 for elog + Discord"
ssh heliosdigios@${mac2020IP} "/Users/heliosdigios/digios/daq/start_run_Mac.sh" &

echo "============= Calling Pi for run status"
ssh ryan@192.168.1.100 "python3 ~/.openclaw/workspace/skills/helios-status/gen_run_status.py" &

echo "============== Opening IOC...."

if [ "$AI_MODE" == "true" ]; then
    export DISPLAY=192.168.1.164:0
    echo "[AI MODE] DISPLAY set to Mac2020 (${DISPLAY})"
fi

cd ${daqDataPath}/${expName}
export TERM=vt100
echo " terminals"
xterm -T ioc1 -geometry 100x20+0+0   -sb -sl 1000 -e "gtReceiver4" "ioc1" "${expName}_run_$DIGIOSRUNNUM.gtd01" "2000000000" "14" &
xterm -T ioc2 -geometry 100x20+0+300 -sb -sl 1000 -e "gtReceiver4" "ioc2" "${expName}_run_$DIGIOSRUNNUM.gtd02" "2000000000" "14" &
xterm -T ioc3 -geometry 100x20+0+600 -sb -sl 1000 -e "gtReceiver4" "ioc3" "${expName}_run_$DIGIOSRUNNUM.gtd03" "2000000000" "14" &
xterm -T ioc4 -geometry 100x20+0+900 -sb -sl 1000 -e "gtReceiver4" "ioc4" "${expName}_run_$DIGIOSRUNNUM.gtd04" "2000000000" "14" &

${HELIOSSYS}/daq/edm/scripts/helios_database start

echo "Run${RUN} Started..."
if [ "$AI_MODE" != "true" ]; then echo "This window will close after 30 sec"
sleep 30; fi
