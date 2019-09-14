#!/usr/bin/python
import serial
import time

# Object accesed datastructure
# 913 --> Gauge 1
# 914 --> Gauge 2
# ?V --> Value-pressure
# structure of command  --> <?V> <-----> <CR>  --> query value
# structure of responce --> <=V> <-----> <sp> <[data]> <CR>

class Route(object):
   def __init__(self):
      self.inst = serial.Serial(port='/dev/ttyUSB1',
                                baudrate=9600,
                                bytesize=serial.EIGHTBITS,
                                parity=serial.PARITY_NONE,
                                stopbits=serial.STOPBITS_ONE,
                                timeout=1
                                )
      if self.inst.isOpen():
         self.inst.close()
         self.inst.open()

   def __del__(self):
      self.inst.close()

def query(route, command):
   inst = route.inst
   inst.write(command)
   out = ''
#      time.sleep(1)
   out += inst.read(50)
   return out

class EdwardsD397(object):

   def __init__(self,route):
      self.route = route

   def gauge_data_extracting(self,data):
      response = data.split()[1]
      data_read = response.split(";")
      return data_read

   def vacuum_g1(self):
      command = "?V"+"913"+"\r"
      value = query(self.route, command)
      value_data = self.gauge_data_extracting(value)[0]
      gauge_data = float(value_data)
      return gauge_data

   def vacuum_g2(self):
      command = "?V"+"914"+"\r"
      value = query(self.route, command)
      return value

   def vacuum_allg(self):
      command = "?V"+"940"+"\r"
      value = query(self.route, command)
      return value

   def vacuum_Punits(self):
      command = "?S"+"929"+"\r"
      value = query(self.route, command)
      return value

