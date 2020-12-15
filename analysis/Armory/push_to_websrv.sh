#!/bin/bash

if [ $# -eq 0 ]; then

   scp Monitors.html ttang@websrv1.phy.anl.gov:/home/ttang/web/helios/Monitors.html

else

   fileName=$1
   msg=$2

   scp "Canvas/"${fileName} ttang@websrv1.phy.anl.gov:/home/ttang/web/helios/Canvas/${fileName}

   sed -i "" $'13 i \\\n <a href="Canvas/'${fileName}'" target="_blank">'${fileName}$'</a> '${msg}$'<p></p>\\\n' Monitors.html 
   scp Monitors.html ttang@websrv1.phy.anl.gov:/home/ttang/web/helios/Monitors.html


fi;


