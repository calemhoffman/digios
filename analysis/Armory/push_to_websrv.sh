#!/bin/bash

websrv="wwwdev.phy.anl.gov"
path="/var/www/html/live/helios/"
key=".ssh/id_ed25519.pub"

if [ $# -eq 0 ]; then

   scp -i ${key} Monitors.html ttang@${websrv}:${path}Monitors.html

else

   fileName=$1
   msg=$2

   scp -i ${key} "Canvas/"${fileName} ttang@${websrv}:${path}Canvas/${fileName}

   sed -i "" $'13 i \\\n <a href="Canvas/'${fileName}'" target="_blank">'${fileName}$'</a> '${msg}$'<p></p>\\\n' Monitors.html 
   scp -i ${key} Monitors.html ttang@${websrv}:${path}Monitors.html

fi;


