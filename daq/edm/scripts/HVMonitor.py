import serial
import serial.tools.list_ports
import time
import os
import numpy
import datetime
import signal
from inputimeout import inputimeout, TimeoutOccurred

HV = (serial.Serial(), serial.Serial(), serial.Serial(), serial.Serial())
HV[0].baudrate = 9600
HV[0].port = '/dev/ttyUSB0'
HV[0].timeout = 1
#print ( HV )
HV[0].open()

def query(mod, str):
	if HV[mod].is_open :
		HV[mod].write(str + "\r\n")
		HV[mod].readline()
		temp = HV[mod].readline()
		temp = temp.replace('\r\n','')
		return temp
	else:
		return "nan"


def set(mod, str):
	if HV[mod].is_open :
		HV[mod].write(str + "\r\n")
		HV[mod].readline()
	else:
		print "Module-%d is not openned." % mod

def setVoltage(mod, ch, v):
	print "Set Module-%d, Channel-%d to %d V" % (mod, ch, v)
	set(mod, ":VOLT %d, (@%d)" %(v, ch))
	print query(mod, "READ:VOLT? (@%d)" % ch)
	print query(mod, "READ:CURR? (@%d)" % ch)

def askVoltCurr(mod, ch):
	v = query(mod, "READ:VOLT? (@%d)" % ch)
	c = query(mod, "READ:CURR? (@%d)" % ch)
	v = float(v.replace('V',''))
	c = float(c.replace('A',''))*1000.
	return (v,c)

def printTable(data):
	(n, ch, q) = data.shape
	for i in range(n):
		print "Module-%d : %s " % (i,ID[i])
	print " "
	msg = "%7s|" % "V[uA]"
	for i in range(n):
		msg += "%10s%-3d|" % ("Module", i)
	print msg
	msg = "-------|"
	for i in range(n):
		msg += "%s+" %  "-------------"
	print msg
	for i in range(ch):
		msg = "%5s-%1d|" % ("ch", i)
		for j  in range(n):
			msg += "%5.0f[%5.2f] |" % (data[j][i][0], data[j][i][1])
		print msg
	msg = "-------+"
	for i in range(n):
		msg += "%s+" %  "-------------"
	print msg
	print "temperature: %.1f C" % temperature[0]


#print query(":READ:MODULE:CHANNELNUMBER?")

def PrintMenu():
	print " "
	dt = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
	print "%s | Table refresh every ~%d sec" % (dt, refreshRate)
	print "------------ control ------------------"
	print "q = Exit            s = Set Voltage"
	print "r = refresh rate "
	print "---------------------------------------"

#CP_BashCommand='scp /home/pi/gaugeDB.txt helios@192.168.1.2:~/.'
#DB_BashCommand='ssh helios@192.168.1.2 curl -sS -i -XPOST "http://heliosdb.onenet:8086/write?db=testing" --data-binary @/home/helios/gaugeDB.txt --speed-time 5 --speed-limit 1000'

def SaveData(data):
	(n, ch, q) = data.shape
	f = open("/home/pi/HVDB.txt",'w')
	for i in range(n):
		for j in range(ch):
			f.write("HV,Module=%d,Det=%d value=%.0f\n" % (i, j, data[i][j][0]))
			f.write("LC,Module=%d,Det=%d value=%.2f\n" % (i, j, data[i][j][1]))
	f.close()
#	os.system(CP_BashCommand)
#	os.system(DB_BashCommand)


ID = []
for mod in range(4):
	ID.append(query(mod, "*IDN?"))
data=numpy.zeros([4,4,2])
temperature=numpy.zeros(4)
refreshRate = 3 #sec

while True:
	for mod in range(4):
		for i in range(4):
			data[mod][i] = askVoltCurr(mod, i)
			temperature[mod] = float(query(mod, ":READ:MODULE:TEMPERATURE?").replace('C', ''))
		#print "ch%d: %.0f V  %.2f uA" % (i, data[0][i][0], data[0][i][1])
	os.system('clear')
	printTable(data)
	PrintMenu()
	try:
		s = inputimeout(prompt='Command : ', timeout=refreshRate)
	except TimeoutOccurred:
		s = 0
		SaveData(data)

	if s == 'q' :
		"bye bye!\n"
		break
	if s == 's' :
		print 'Channel ?'
		ch = input()
		print 'Voltage for ch-%d [V]?' % ch
		v = input()
		setVoltage(ch,v)
	if s == 'r' :
		print 'New refresh rate [sec]?'
		refreshRate = input()

