#!/usr/local/bin/python3
import os
import time
import datetime
import sys
from influxdb import InfluxDBClient

arg = sys.argv
#rint(sys.argv)

#set database server
client = InfluxDBClient(host='localhost', port=8086)

#choose database
client.switch_database('testing')


print("==========================================================")
print("==               Get InfluxDB data                      ==")
print("==========================================================")


if len(arg) == 2 and arg[1] == "list":
   results = client.query('show measurements')
   print("")
   print("---------- list of query")
   for pt in results.get_points() :
      print(pt['name'])
   print("=========================================== end of program")
   sys.exit()
   

if len(arg) < 4 :
   print("usage : ./GetInfluxDBData.py [time_start] [time_end] [query]")
   print("     ")
   print(' [time] = "now() - XXX", XXX = 30s, 1m, 2h, 2d, 1d2h3m4s ') 
   print('        = 2020-04-06T:23:00:00Z') 
   print("")
   print(' [query] = hitRate ')
   print('         = VacuumGauge')
   print('         = sumHitX')
   print('         = sumHitY')
   print('         = threshold')
   print('         = router1')
   print('         = router2')
   print('         = fileSize')
   print('         = RunNum')
   print('      * to know the complete list, ./GetInfluxDBData.py list')
   print('')
   sys.exit()


# format query
relTime1 = False
relTime2 = False

if arg[1].find("now") >= 0 :
      relTime1 = True
      
if arg[2].find("now") >= 0 :
      relTime2 = True
   
if( relTime1==False and relTime2==False ) :
   print("---------hahdha")
   query="SELECT * FROM %s WHERE \'%s\' < time AND time < \'%s\' " % (arg[3], arg[1], arg[2])
   
if( relTime1==False and relTime2==True) :
   query="SELECT * FROM %s WHERE \'%s\' < time AND time < %s " % (arg[3], arg[1], arg[2])

if( relTime1==True and relTime2==False) :
   query="SELECT * FROM %s WHERE %s < time AND time < \'%s\' " % (arg[3], arg[1], arg[2])

if( relTime1==True and relTime2==True) :
   query="SELECT * FROM %s WHERE %s < time AND time < %s " % (arg[3], arg[1], arg[2])

print(query)


#get number of items
test=client.query("Select * from %s order by desc limit 1" % arg[3] );
items = []
for pt in test.get_points():
   for i in pt :
      items.append(i)
print(items)


#get query
results = client.query( query )

#print(results.raw)

print("================================== 1st 10 entry:");

fileName = "DB_%s.txt" % arg[3]
f=open(fileName, 'w');


f.write("#%13s, %15s," % ("Year", "Time"))
for i in items:
   if i != 'time' and i != 'value' :
      f.write("%s, " % i )
   if i == 'value' :
      f.write("%12s\n" % arg[3] )

count=0
for pt in results.get_points():
   if count < 10 :
      print(pt)
   count = count+1
      
   datetimeStr=pt['time']
   pos=datetimeStr.find('T')
   dateStr=datetimeStr[0:pos]
   timeStr=datetimeStr[pos+1:-1]
   
   #if arg[3] == "hitRate" or arg[3] == "threshold" :
   #   f.write("%14s, %15s, %3s, %3s, %2s, %14d\n" % (dateStr, timeStr, pt['VME'], pt['DIG'], pt['CH'], pt['value']))
   #else :
   f.write("%14s, %15s, " % (dateStr, timeStr))
   for i in items:
      if i != 'time' and i != 'value':
         f.write("%s, " % pt[i] )
      if i == 'value' :
         f.write("%12d\n" % pt[i] )
print("------------------------------------------- end of entry")
f.close()

print(" number of Entry found : %d" % count );
print(" file saved in %s " % fileName)
print("=========================================== end of program")
