import serial
import serial.tools.list_ports
import time
import os
import numpy
import datetime
import signal
import thread
import math
from inputimeout import inputimeout, TimeoutOccurred

HV = (serial.Serial(), serial.Serial(), serial.Serial(), serial.Serial())
HV[0].baudrate = 9600
HV[0].port = '/dev/ttyUSB0'
HV[0].timeout = 1

HV[1].baudrate = 9600
HV[1].port = '/dev/ttyUSB1'
HV[1].timeout = 1

HV[2].baudrate = 9600
HV[2].port = '/dev/ttyUSB2'
HV[2].timeout = 1

HV[3].baudrate = 9600
HV[3].port = '/dev/ttyUSB3'
HV[3].timeout = 1

for mod in range(4):
	try :
		HV[mod].open()
	except:
		HV[mod].is_open = False

arduino = serial.Serial('/dev/ttyACM0', baudrate = 9600, timeout = 5)

def readArduino():
	msg = arduino.readline()
	msg = msg.replace('\r\n', '')
	msg = msg.replace('\t', ',')
	return msg

def toFloat(x):
	try :
		return float(x)
	except ValueError:
		return float("nan")

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
	if HV[mod].is_open == False:
		try:
			HV[mod].open()
			ID[mod] = query(mod, "*IDN?")
		except:
			HV[mod].is_open = False
			print "cannot open HV module %d" % mod
	if HV[mod].is_open :
		HV[mod].write(str + "\r\n")
		HV[mod].readline()
	else:
		print "Module-%d is not openned." % mod

def setVoltage(mod, ch, v):
	print "Set Module-%d, Channel-%d to %d V" % (mod, ch, v)
	set(mod, ":VOLT %d, (@%d)" %(v, ch))
	print query(mod, ":READ:VOLT? (@%d)" % ch)
	print query(mod, ":READ:CURR? (@%d)" % ch)

def askVoltCurr(mod, ch):
	v = query(mod, ":MEAS:VOLT? (@%d)" % ch)
	c = query(mod, ":MEAS:CURR? (@%d)" % ch)
	v = float(v.replace('V',''))
	c = float(c.replace('A',''))*1000.
	return (v,c)

def askSetVoltCurr(mod, ch):
	v = query(mod, ":READ:VOLT? (@%d)" % ch)
	c = query(mod, ":READ:CURR? (@%d)" % ch)
	v = float(v.replace('V',''))
	c = float(c.replace('A',''))*1000.
	return (v,c)

def askRampSpeed(mod, ch):
	s = query(mod, ":READ:RAMP:VOLT? (@%d)" % ch)
	s = float(s.replace('V/s',''))
	of = query(mod, ":READ:VOLT:ON? (@%d)" % ch)
	of = int(of)
	return (s, of)

def setRampOn(mod,ch):
	set(mod, ":VOLT ON,(@%d)" % ch)

def setBraggVoltage(detID, v):
	setVoltage(0,detID,v)
	setVoltage(1,detID,v)
	setVoltage(2,detID,v)

def setMWDCVoltage(v):
	for i in range(4):
		setVoltage(4,i,v)

def printModule(ID):
	print "=========================================="
	for i in range(4):
		print "Module-%d : %s " % (i,ID[i])

def printVoltage(data, title):
	(n, ch, q) = data.shape
	print "\n--------------- %s" % title
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

def printTemperature():
	for i in range(4):
		print "temperature-%d: %.1f C" % (i,temperature[i])

def printPressure(pressure):
	print " "
	print "Pressure:"
	print "------------------------------"
	print "          MWDC      |          Bragg"
	for i in range(4) :
		try:
			print "Det-%d : %6.2f mbar | Det-%d : %6.2f mBar" % (2*i, toFloat(pressure[2*i]), 2*i+1, toFloat(pressure[2*i+1]))
		except:
			print "Det-%d : %6s mbar | Det-%d : %6s mBar" % (2*i, "nan", 2*i+1, "nan")

#print query(":READ:MODULE:CHANNELNUMBER?")

def PrintMenu():
	print " "
	dt = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
	print "%s | Table refresh every ~%d sec" % (dt, refreshRate)
	print "------------ control ------------------"
	print "q = Exit            s = Set Voltage"
	print "b = Bragg Voltage   m = MWDC Volatge"
	print "V = print Set Voltage"
	print "R = print RAMP status"
	print "c = SCPI Command"
	print "---------------------------------------"

CP_BashCommand='scp -q /home/pi/gaugeDB.txt helios@192.168.1.2:~/.'
DB_BashCommand='ssh helios@192.168.1.2 curl -sS -i -XPOST "http://heliosdb.onenet:8086/write?db=testing" --data-binary @/home/helios/gaugeDB.txt --speed-time 5 --speed-limit 1000 --max-time 5 >> /dev/null'

def SaveData(data, pressure):
	(n, ch, q) = data.shape
	f = open("/home/pi/gaugeDB.txt",'w')
	for i in range(n):
		for j in range(ch):
			if math.isnan(data[i][j][0]) == False :
				f.write("HV,Module=%d,Det=%d value=%.0f\n" % (i, j, data[i][j][0]))
			if math.isnan(data[i][j][1]) == False :
				f.write("LC,Module=%d,Det=%d value=%.2f\n" % (i, j, data[i][j][1]))
	if len(pressure) == 13 :
		for i in range(8):
			v = toFloat(pressure[i])
			if( v>0 and v < 1000 ):
				f.write("Gauge,Det=%d value=%.2f\n" % (i, v))

	f.close()
	os.system(CP_BashCommand)
	os.system(DB_BashCommand)


ID = []
RAMP = numpy.zeros([4,4,2])
for mod in range(4):
	ID.append(query(mod, "*IDN?"))
	for ch in range(4):
		setRampOn(mod, ch)
		RAMP[mod][ch] = askRampSpeed(mod, ch)

printVoltage(RAMP, "RAMP V/s [On/off]")

data=numpy.zeros([4,4,2])
setData=numpy.zeros([4,4,2])
temperature=numpy.zeros(4)
refreshRate = 5 #sec

while True:
	start_time = time.time()
	for mod in range(4):
		for i in range(4):
			data[mod][i] = askVoltCurr(mod, i)
		temperature[mod] = float(query(mod, ":READ:MODULE:TEMPERATURE?").replace('C', ''))
		#print "ch%d: %.0f V  %.2f uA" % (i, data[0][i][0], data[0][i][1])
	a_time = time.time()
	pressure = readArduino()
	if len(pressure) == 0 :
		print "reopen arduino"
		arduino.close()
		arduino.open()
		continue
	if( pressure.find('OK:') == - 1 or pressure.rfind('OK:') != 0 ) :
		continue
	pressure = pressure[3:-1]
	pressure = pressure.split(",")
	b_time = time.time()
	os.system('clear')
	printModule(ID)
	printVoltage(data, "Measure V[uA]")
	printTemperature()
	printPressure(pressure)
	PrintMenu()
	print "arduino reading time [sec] : ", b_time - a_time
	waitTime = (time.time() - start_time)
	print "  total reading time [sec] : ", waitTime
	waitTime = refreshRate - waitTime
	if( waitTime < 0 ) :
		waitTime = refreshRate
	try:
		s = inputimeout(prompt='Command : ', timeout=waitTime)
	except TimeoutOccurred:
		s = 0
		thread.start_new_thread( SaveData, (data, pressure))

	if s == 'q' :
		"bye bye!\n"
		break
	if s == 's' :
		mod = input("which module ? ")
		ch = input("Which Channel ? ")
		v = input("Voltage for ch-%d [V]" % ch)
		setVoltage(mod,ch,v)
	if s == 'r' :
		refreshRate = input("New refresh Rate [sec]?")
	if s == 'b' :
		detID = input("Bragg ID [0,1,2,3] ?")
		v = input("Voltage [V] ?")
		setBraggVoltage(detID,v)
	if s == 'm' :
		v = input("MWDC Voltage [V] ?")
		setMEDCVoltage(v)
	if  s == 'c' :
		mod = input("Module ? ")
		cmd = input("SCPI Command [don't forget \" \"] ?")
		print  query(mod, cmd)
		dump = raw_input("press any key to continue")
	if s == 'R' :
		for mod in range(4):
			for ch in range(4):
				RAMP[mod][ch] = askRampSpeed(mod, ch)
		printVoltage(RAMP, "RAMP V/s [On/Off]")
		dump = raw_input("press any key to continue")
	if s == 'V' :
		for mod in range(4):
			for ch in range(4):
				setData[mod][i] = askSetVoltCurr(mod, i)
		printVoltage(setData,"Set Voltage Current V [uA]")
		dump = raw_input("press any key to continue")
