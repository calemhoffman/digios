#!/usr/bin/python

from epics import caget
from epics import caput
import os
import time
import datetime

print("======== Auto-Tune Threshold ========")

VME=2
DIG=1
CH=0

pv="VME0%d:MDIG%d:channel_enable%d" % (VME, DIG, CH)
isChEnabled=caget(pv)

print( isChEnabled )

if( isChEnabled ) :
    # get last 10 trigger rate from database

'''
for VME in range (1,5):
    pv="DAQC%d_CV_BuffersAvail" % (VME)
    result=caget(pv)
        #print(type(result))
    print("VME%d Buffer %s" % (VME, result))
    
        #if VME == 2: 
        #    result = 290.0
        #    print("====== VME%d Buffer %s" % (VME, result))

    if ( isinstance(result, float) or isinstance(result, int) ) :
        if ( float(result) < 300 ) :
            caput("Online_CS_StartStop", "Stop")
            gf=open("/home/helios/helioBuffer.log", "a")
            dt = datetime.datetime.now()
            gf.write("  stop : %s\n" % dt)
            print("================ stop ACQ")
            waitSec = 5
            print("wait for %d sec" % waitSec)
            for i in range (1, waitSec):
                time.sleep(1)
                print(i)
                caput("Online_CS_StartStop", "Start")
                dt = datetime.datetime.now()
                gf.write("resume : %s\n" % dt)
                print("================= resume")
                gf.close()

            string="buffer,VME=%d value=%s\n" % (VME, result)
            f.write(string)

            for DIG in range (1,5):
                for CH in range(0, 10):
                    pv="VME0%d:MDIG%d:disc_count%d_RBV" % (VME, DIG, CH)
                    result=caget(pv)
                    string="hitRate,VME=%d,DIG=%d,CH=%d value=%s\n" % (VME, DIG, CH, result)
                    f.write(string)
                
                    pv="VME0%d:MDIG%d:led_threshold%d" % (VME, DIG, CH)
                    result=caget(pv)
                    string="threshold,VME=%d,DIG=%d,CH=%d value=%s\n" % (VME, DIG, CH, result)
                    f.write(string)

        else :
            string="buffer,VME=%d value=-100\n" % (VME)
            f.write(string)
'''



