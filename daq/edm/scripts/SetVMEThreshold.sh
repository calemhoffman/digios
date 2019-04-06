#!/bin/bash

if [ $# -eq 0 ] || [ $# -ne 3  ]; then
    echo "Please input VME, DIG and CH"
    exit 1
fi

VME=$1
DIG=$2
CH=$3

echo " Set Thredhold values for VME-${VME}:${DIG}:CH${CH}"

threshold=$(caget VME0${VME}:MDIG${DIG}:led_threshold${CH} | awk '{print $2}')
dataStr="INSERT threshold,VME=${VME},DIG=${DIG},CH=${CH} value=${threshold}"
influx -execute "${dataStr}" -database="testing"

echo " === current threshold is ${threshold}"

read -p " === Set threshold to ? " newThreshold

caput VME0${VME}:MDIG${DIG}:led_threshold${CH} ${newThreshold}

dataStr="INSERT threshold,VME=${VME},DIG=${DIG},CH=${CH} value=${newThreshold}"
influx -execute "${dataStr}" -database="testing"

echo "========= done."