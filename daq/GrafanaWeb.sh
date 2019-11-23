#!/bin/bash


echo "============ taking screen shot"

screenShot=/home/heliosdatabase/grafanaElog.jpg
#gnome-screenshot --file=${screenShot}
import -window root ${screenShot}
convert ${screenShot} -crop 1980x1080+0+0 ${screenShot}

echo "============ push to anl web"
scp -rp ${screenShot} ttang@websrv1.phy.anl.gov:/home/ttang/web/helios/. && echo "============ success."
