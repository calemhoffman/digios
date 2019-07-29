#!/bin/bash


echo "============ taking screen shot"

screenShot=/home/heliosdatabase/grafanaElog.jpg
gnome-screenshot --file=${screenShot}
#import -window root ${screenShot} #this command will capture local screen, not remote screen
width=$(identify -format '%w' ${screenShot})
higth=$(identify -format '%h' ${screenShot})

echo "ScreeShot size : ${width} x ${higth}"

convert ${screenShot} -crop 1980x1080+0+0 ${screenShot}

echo "============= downloading expName.sh"
rsync -avuht --progress "helios@digios1:~/digios/expName.sh" ~/digios/expName.sh
rsync -avuht --progress "helios@digios1:~/digios/analysis/working/elog_context.txt" ~/elog.txt
source ~/digios/expName.sh
echo "============= push to elog for Grafana"

#elog -h www.phy.anl.gov -d elog -p 443 -l ${expName^^} -s -u GeneralHelios helios -a Category=Run -a RunNo=${LastRunNum} -a Subject="Grafana ScreenShot for ${LastRunNum}" -f ${screenShot} -n 1 "Grafana auto screenshot"

#the folllowing is use for replacing the elog. more, see digios1:~/digios/daq/edm/scripts/stop_run.sh
elog -h www.phy.anl.gov -d elog -p 443 -l ${expName^^} -s -u GeneralHelios helios -e ${elogID} -f ${screenShot} -n 1 ~/elog.txt

