#!/bin/bash


echo "============ taking screen shot"

#=== for linux
#screenShot=/home/heliosdatabase/grafanaElog.jpg
#gnome-screenshot --file=${screenShot}
#import -window root ${screenShot}
#convert ${screenShot} -crop 1980x1080+0+0 ${screenShot}


#=== for mac

screenShot=~/grafanaElog.jpg

screencapture -D2 ${screenShot}

echo "============ push to anl web"
scp -rp ${screenShot} ttang@websrv1.phy.anl.gov:/home/ttang/web/helios/live/. && echo "============ success."
