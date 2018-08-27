#!/bin/bash
ATTEMPTS=0

while [ "${ATTEMPTS}" -lt "100" ];o
do
    rsync -rtuh --progress rsync://helios@anldaqrouter:12000/digiosdata /Users/calemhoffman/Research/isolde/data

    # --delete
    #/Users/heliosdigios/experiments/infl001_19O/data/.
    ATTEMPTS=$(expr ${ATTEMPTS} + 1)
sleep 10
done
