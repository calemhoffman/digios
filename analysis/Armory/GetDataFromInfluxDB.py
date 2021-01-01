#!/usr/bin/python3

VME=1
DIG=2
CH=8

data = "hitRate"

startTime = "2020-12-21T17:00:00Z"
endTime   = "2020-12-23T18:00:00Z"

saveFile="IC00.txt"

#=========================================== code

print("===========================")

str = 'SELECT "value" FROM \"%s\" WHERE time > \'%s\' AND time < \'%s\' AND "VME" = \'%d\' AND "DIG" = \'%d\' AND "CH" = \'%d\' tz(\'America/Chicago\') ' % (data, startTime, endTime, VME, DIG, CH)

print(str)

#=========================================== code
from influxdb import InfluxDBClient

client = InfluxDBClient(host='localhost', port=8086)

client.switch_database('testing')

result = client.query(str)
#result = client.query('SELECT "value" FROM "hitRate" WHERE time > \'2020-12-22T20:00:00Z\' AND time < \'2020-12-22T20:01:00Z\' AND "VME" = \'1\' AND "DIG" = \'2\' AND "CH" = \'8\' ')

print("=====================")

points = result.get_points()

f = open(saveFile, 'w')

for point in points:
    time = point['time'] 
    oStr = "%s   %s   %i\n" % (time[0:10], time[11:-1], point['value'])
    #print(oStr)
    f.write(oStr)

f.close()

print("=================== done, saved --> %s " % saveFile )


