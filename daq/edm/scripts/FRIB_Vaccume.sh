#!/bin/bash -l

source ${HELIOSSYS}/daq/edm/scripts/DataBaseAddress.sh

while [ 1 ]; do

    readTxt=$(~/sshFRIB.sh VaccumeGauga.sh | tail -n1)
    reading=${readTxt//$'\015'}
    echo $(date)" | "$reading "[mBar?]"
    curl -sS  -XPOST "http://${dataBaseAddress}:8086/write?db=testing" --data-binary "VacuumGauge value=${reading}" --speed-time 5 --speed-limit 1000

    sleep 60

done