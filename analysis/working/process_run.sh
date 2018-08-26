#!/bin/sh

if [ $# -eq 0 ]
then
    read -p 'Please enter the run number you would like to process: ' RUN
    read -p 'Please enter the system you are running on (1-MAC, 2-ONENET): ' WHERE
elif [ $# -eq 1 ]
then
    RUN=$1
    WHERE=1
else
    RUN=$1
    WHERE=$2
fi

ATTEMPTS=0

while [ "${ATTEMPTS}" -lt "10000" ];
do
    
    rsync -rtuh --delete --progress rsync://helios@192.168.1.2:12000/digiosdata /Users/heliosdigios/experiments/infl001_19O/data/.
    #not needed when sync daemon running
    #/Users/heliosdigios/Applications/get_digios_data.sh $RUN $WHERE
    /Users/heliosdigios/experiments/infl001_19O/working/gebmerge.sh $RUN
    /Users/heliosdigios/experiments/infl001_19O/working/gebsortmerged.sh $RUN
    
    echo Just created root file run${RUN}.root in ../root_data/
    
    root -q -b "process_run.C(${RUN},0)"
    cp gen.root ../root_data/gen_run${RUN}.root
    echo copied gen.root to gen_run${RUN}.root
    
    echo ----Done with Processing Run Number ${RUN}----
    echo "process event attempt number = ${ATTEMPTS}"
    
    ATTEMPTS=$(expr ${ATTEMPTS} + 1)
    sleep 5
    
done

