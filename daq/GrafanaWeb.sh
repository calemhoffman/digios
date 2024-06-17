#!/bin/bash


echo "============ taking screen shot"

#=== for linux
#screenShot=/home/heliosdatabase/grafanaElog.jpg
#gnome-screenshot --file=${screenShot}
#import -window root ${screenShot}
#convert ${screenShot} -crop 1980x1080+0+0 ${screenShot}


#=== for mac

screenShot=~/grafanaElog.jpg

#--- when no big TV for Grafana
# following script for finding safari-ID
#safariID=$(sudo osascript -e 'tell app "Safari" to id of window 1')
#echo "safariID = ${ID}"
#--- this capture safari screenshot
#safariID=1002
#echo "Safari window ID = "${safariID}
#screencapture -l${safariID} ${screenShot}
#~/scpScreenShot.sh ${screenShot}    

#--- when back to ANL
screencapture -D2 ${screenShot}
echo "============ push to anl web"
scp -rp ${screenShot} ttang@wwwdev.phy.anl.gov:/var/www/html/live/helios/. && echo "============ success."
#echo -e "\033[1;31m Push to websrv1 is disabled. Please edit mac2017:~/digios/daq/GrafanaWeb.sh\033[m"
