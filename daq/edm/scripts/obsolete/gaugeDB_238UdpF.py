import serial
import serial.tools.list_ports
import time
import os
import numpy
import datetime

arduino = serial.Serial()
arduino.baudrate = 9600
arduino.port = '/dev/ttyACM0'
arduino.timeout = 10

print ( arduino)

arduino.open()

#gv = numpy.empty([4,2])
#print gv

CP_BashCommand='scp /home/pi/gaugeDB.txt helios@192.168.1.2:~/.'
DB_BashCommand='ssh helios@192.168.1.2 curl -sS -i -XPOST "http://heliosdb.onenet:8086/write?db=testing" --data-binary @/home/helios/gaugeDB.txt --speed-time 5 --speed-limit 1000'

def toFloat(v):
	try :
		return float(v)
	except ValueError:
		return -1

while 1:
	dt = datetime.datetime.now().strftime("%Y-%m-%d %H:%H:%S")
	print dt

	msg = arduino.readline()
	msg = msg.replace('\r','')
	msg = msg.replace('\n','')
	msg = msg.replace('\t',',')

	print len(msg)
	print msg

	if( len(msg) == 0 ):
		print "reopen arduino"
		arduino.close()
		arduino.open()
		continue

	if( msg.find('OK:') == -1 or msg.rfind('OK:') != 0 ) :
		continue

	msg = msg[3:-1]

	value = msg.split(",")

	#print value[0:8]

	if len(value) == 12 :
		f = open("/home/pi/gaugeDB.txt",'w')
		for i in range(8):
			v = toFloat(value[i])
			print "%d : %f" % (i, v)
			if( v > 0 and v < 1000 ):
				f.write("Gauge,Det=%d value=%.2f\n" % (i,v));
		f.close()
		os.system(CP_BashCommand)
		os.system(DB_BashCommand)

	time.sleep(2) #actual time will be +3
