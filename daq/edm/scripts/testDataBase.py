#!/usr/bin/python

import os
import time

print("======== DataBase for PSD Array ========")

VME = 1
t1 = int(round(time.time() * 1000 ))
print t1
for VME in range (1,5):
    for DIG in range (1,4):
        for CH in range(0, 10):
            bashCommand="caget VME0%d:MDIG%d:disc_count%d_RBV | awk '{print $2}'" % (VME, DIG, CH)
            #print(bashCommand)
            result = os.popen(bashCommand).read().rstrip()
            print "----- VME:",VME,":DIG:", DIG, ":CH:", CH,":hitRate:", result
            #bashCommand='influx -execute "INSERT threshold,VME=%d,DIG=%d,CH=%d value=%s" -database="testing"'  % (VME, DIG, CH, result)
            bashCommand='curl -sS -XPOST "http://heliosdb.onenet:8086/write?db=testing" --data-binary "hitRate,VME=%d,DIG=%d,CH=%d value=%s" --max-time 2' % (VME, DIG, CH, result)
            #print(bashCommand)
            os.system(bashCommand)
t2 = int(round(time.time() * 1000 ))
print t2, ", diff = ", t2-t1 , " mses"
