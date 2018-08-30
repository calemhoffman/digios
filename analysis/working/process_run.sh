#!/bin/sh

if [ $# -eq 0 ]
then
    read -p 'Please enter the run number you would like to process: ' RUN
fi

RUN=$1
#ATTEMPTS=0

#while [ "${ATTEMPTS}" -lt "10000" ];
#do

#rsync -rtuh --delete --progress rsync://helios@anldaqrouter:12000/digiosdata /Users/calemhoffman/Research/isolde/data/.
# 
#not needed when sync daemon running
#/Users/heliosdigios/Applications/get_digios_data.sh $RUN $WHERE
#rsync -rtuavh --delete --progress helios@anldaqrouter:/media/DIGIOSDATA3/data/iss000/*run_${RUN}* /Users/heliosdigios/experiments/iss000/data/*

/Users/heliosdigios/Applications/get_digios_data.sh $RUN 3
/Users/heliosdigios/experiments/iss000/analysis/working/gebmerge.sh $RUN
/Users/heliosdigios/experiments/iss000/analysis/working/gebsortmerged.sh $RUN

echo Just created root file run${RUN}.root in /Users/heliosdigios/experiments/iss000/root_data/
ls -ltrh /Users/heliosdigios/experiments/iss000/root_data

root -q -b "process_run.C(${RUN},0)"
cp gen.root /Users/heliosdigios/experiments/iss000/root_data/gen_run${RUN}.root
echo copied gen.root to gen_run${RUN}.root

echo ----Done with Processing Run Number ${RUN}----
#    echo "process event attempt number = ${ATTEMPTS}"

#    ATTEMPTS=$(expr ${ATTEMPTS} + 1)
#    sleep 5
    
#done

