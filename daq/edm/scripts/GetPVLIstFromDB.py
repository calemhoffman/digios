#!/usr/bin/python

print("======== PV List for HELIOS ========")

#heliosPath=os.environ["HELIOSSYS"]

PVPath="/global/devel/gretTop/9-22/dgsIoc/db/"

#PVFile="dgsGlobals_HELIOS_VME01.db"
#PVFile="dgsGlobals_HELIOS_GLBL.db"

PVFile="dgsMTrigRegisters.template"
PVFile="dgsRTrigUser.template"
PVFile="dgsRTrigRegisters.template"
PVFile="dgsGlobals_DGS_GLBL.db"
PVFile="dgsDigRegisters.template"

fexp=open("%s%s" % (PVPath, PVFile), 'r')

pvList=[]

totalPV = 0

for line in fexp:
    line = line.strip('\n')
    if( line[:6] == "record"):
        pos = line.find('"') + 1
        pos2 = line.rfind('"')
        pvList.append(line[pos:pos2])
        totalPV = totalPV + 1
        print("%s" % line[pos:pos2])

fexp.close()

print(" total PV found : %d " % totalPV )

print("=========================== Global PV")

tag = [0]*totalPV

counter = 0
globalPV = 0
for pv in pvList:
    if ( pv[6:10] == "GLBL" ) :
        print("%s" % pv)
        globalPV = globalPV + 1
        tag[counter] = 1
    counter = counter + 1

print(" Gloabl PV found : %d " % globalPV )


print("=========================== MDIG PV")

MDIGPV = 0
MDIGPV_F2  = 0
counter = 0
for pv in pvList:
    if ( pv[6:11] == "MDIG1" and pv[12:14] != "F2") :
        print("%s" % pv)
        MDIGPV = MDIGPV + 1
        tag[counter] = 2
    if ( pv[6:11] == "MDIG1" and pv[12:14] == "F2" ) :
        #print("---- %s" % pv)
        MDIGPV_F2 = MDIGPV_F2 + 1
        tag[counter] = 3
    if ( pv[6:10] == "MDIG" and pv[10:11] != "1") :
        tag[counter] = 4
    counter = counter + 1

print(" MDIG PV found : %d " % MDIGPV )
print(" MDIG_F2 PV found : %d " % MDIGPV_F2 )


print("============================= Other PV")

OtherPV = 0
counter = 0
for pv in pvList:
    if ( tag[counter] == 0) :
        print("%s" % pv)
        OtherPV = OtherPV + 1
    counter = counter + 1

print(" Other PV found : %d " % OtherPV )

print("====================================")

print(" total PV found   : %d " % totalPV )
print(" Gloabl PV found  : %d " % globalPV )
print(" MDIG PV found    : %d " % MDIGPV )
print(" MDIG_F2 PV found : %d " % MDIGPV_F2 )
print(" Other PV found   : %d " % OtherPV )



