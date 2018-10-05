#!/bin/bash

#code to continually check the buffer number in the ioc to try 
# and catch daq crashes - written poorly by CRH - 9/17

echo -n ""
ATTEMPTS=0

buffers_check() {
chpv=$(caget Online_CS_StartStop | awk '{print $2}')
if [ "${chpv}" == "Start" ]
then

    while [ "${ATTEMPTS}" -lt "200000" ];
    do	
	echo Buffer Monitor Check Number ${ATTEMPTS}
	for a in 1 2 3 4
	do
	    chpv=$(caget DAQC"$a"_CV_BuffersAvail | awk '{print $2}')
	    if [ "${chpv}" -lt "350" ]
	    then
		echo ----- Buffers Low Stopping Run -----
		caput Online_CS_StartStop Stop
	    #else
		#echo OK $a
	    fi
	done
	ATTEMPTS=$(expr ${ATTEMPTS} + 1)
	sleep 3
	
    done
fi
}

cmd=0
if [ $# -eq 1 ]
then
    cmd=$1
    case $cmd in
	1) buffers_check ;;
    esac
    exit
fi
