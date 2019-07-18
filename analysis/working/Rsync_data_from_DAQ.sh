#!/bin/sh

PCName="$(hostname)"

if [ ${PCName:0:5} == "bebop" ]; then
    echo " need to do ssh via sonata "
    echo " OR, using Globus"
    exit
fi 

if [ ${PCName} = "phywl158.phy.anl.gov" ]; then
    source ~/digios/expName.sh
    dir=~/experiments/${expName}/data
    mkdir -v ${dir}

    #remote data path
    dataloc=${daqDataPath}/${expName}/data
    #===== directory
    IP=192.168.1.2
    rsync -avuht --progress "helios@${IP}:${dataloc}/${expName}_run_*" ${dir}/.    
fi

if [ $PCName = "digios1" ] ; then
    echo "already in digios1."
fi

exit
