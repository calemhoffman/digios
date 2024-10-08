#!/usr/local/bin/python2.7

import os
import serial
import time
import datetime
import Edwards_D379_driver

route = Edwards_D379_driver.Route()
gaude_read = Edwards_D379_driver.EdwardsD397(route)

heliosPath=os.environ["HELIOSSYS"]

fexp=open("%s/daq/edm/scripts/DataBaseAddress.sh" % heliosPath, 'r')
line=fexp.readline() #this line is bashscript header
#line=fexp.readline() #this line is dataBaseAddress
pos=line.find("=")
dataBaseAddress=line[pos+1:-1]
fexp.close()

print(dataBaseAddress)    

while 1:

   output1 = gaude_read.vacuum_g1()/100.
   #output2 = gaude_read.vacuum_g2()/100.
   #output3 = gaude_read.vacuum_Punits()
   output4 = gaude_read.vacuum_allg()

   dt=datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
   
   print  "%s| %f mbar." % (dt,output1)
   #print "VacuumGauge = %s mbar." % gaude_read.vacuum_g2()
   #print output4

   DB_command = 'curl -s -XPOST "http://%s:8086/write?db=testing" --data-binary "VacuumGauge value=%f" --max-time 1 --connect-timeout 1' % (dataBaseAddress,output1)

   os.system(DB_command)

   time.sleep(3)
#   print output2
