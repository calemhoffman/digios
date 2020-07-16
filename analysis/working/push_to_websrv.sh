#!/bin/bash

fileName=$1

scp "Canvas/"${fileName} ttang@websrv1.phy.anl.gov:/home/ttang/web/helios/Canvas/${fileName}

sed -i "" $'10 i \\\n <a href="Canvas/'${fileName}'">'${fileName}$'</a><p></p>\\\n' Monitors.html 
scp Monitors.html ttang@websrv1.phy.anl.gov:/home/ttang/web/helios/Monitors.html
