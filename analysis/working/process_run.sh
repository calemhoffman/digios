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

    #rsync -rtuh --delete --progress rsync://helios@anldaqrouter:12000/digiosdata /Users/calemhoffman/Research/isolde/data/.
   # 
    #not needed when sync daemon running
    #/Users/heliosdigios/Applications/get_digios_data.sh $RUN $WHERE
    rsync -rtuh --delete --progress helios@anldaqrouter:/media/DIGIOSDATA3/data/iss000/* /Users/calemhoffman/Research/isolde/data/*
    /Users/calemhoffman/Research/isolde/iss_git/analysis/working/gebmerge.sh $RUN
    /Users/calemhoffman/Research/isolde/iss_git/analysis/working/gebsortmerged.sh $RUN
    
    echo Just created root file run${RUN}.root in /Users/calemhoffman/Research/isolde/root_data/
    ls -ltrh /Users/calemhoffman/Research/isolde/root_data
    
    root -q -b "process_run.C(${RUN},0)"
    cp gen.root /Users/calemhoffman/Research/isolde/root_data/gen_run${RUN}.root
    echo copied gen.root to gen_run${RUN}.root
    
    echo ----Done with Processing Run Number ${RUN}----
    echo "process event attempt number = ${ATTEMPTS}"
    
    ATTEMPTS=$(expr ${ATTEMPTS} + 1)
    sleep 5
    
done

