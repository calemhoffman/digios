#!/bin/bash

fileName=$1
msg=$2

scp "Canvas/"${fileName} ttang@websrv1.phy.anl.gov:/home/ttang/web/helios/Canvas/${fileName}

sed -i "" $'13 i \\\n <a href="Canvas/'${fileName}'" target="_blank">'${fileName}$'</a> '${msg}$'<p></p>\\\n' Monitors.html 
scp Monitors.html ttang@websrv1.phy.anl.gov:/home/ttang/web/helios/Monitors.html
