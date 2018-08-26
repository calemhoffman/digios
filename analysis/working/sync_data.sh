#!/bin/bash
ATTEMPTS=0

while [ "${ATTEMPTS}" -lt "100" ];
do
    rsync -rtuh --delete --progress rsync://helios@192.168.1.2:12000/digiosdata /Users/heliosdigios/experiments/infl001_19O/data/.
    ATTEMPTS=$(expr ${ATTEMPTS} + 1)
sleep 5
done
