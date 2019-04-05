#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Please enter waiting time in sec"
    exit 1
fi

time=$1

VME="1 2 3 4"
DIG="1 2 3"
CH="0 1 2 3 4 5 6 7 8 9"

loop=1

while [ ${loopflag} -eq 1 ]; do
    for a in $VME ; do
	for b in $DIG ; do
	    for c in $CH ; do
		threshold=$(caget VME0${a}:MDIG${b}:led_threshold${c} | awk '{print $2}')
		dataStr="INSERT threshold,VME=${a},DIG=${b},CH=${c} value=${threshold}"
		echo ${dataStr}
		influx -execute "${dataStr}" -database="testing"
		sleep 0.5
	    done
	done
    done

    sleep time

done