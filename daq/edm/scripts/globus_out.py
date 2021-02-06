#!/usr/local/bin/python2.7

import globus_sdk
import sys

#this is how to get the token.
CLIENT_ID = '1f2df281-39e0-4f75-b377-f11ae435a7e8'

client = globus_sdk.NativeAppAuthClient(CLIENT_ID)

import os

print("===================== this script only for single run")

f = open("/home/helios/Globus-token.dat", "r");
AUTH_TOKEN = f.readline().strip()
TRANSFER_TOKEN = f.readline().strip()
f.close()

#print( TRANSFER_TOKEN )

#authorizer = globus_sdk.AccessTokenAuthorizer(TRANSFER_TOKEN)
authorizer = globus_sdk.RefreshTokenAuthorizer(
    TRANSFER_TOKEN, client, access_token=AUTH_TOKEN)

tc = globus_sdk.TransferClient(authorizer=authorizer)

heliosPath=os.environ["HELIOSSYS"]
fexp=open("%s/expName.sh" % heliosPath, 'r')
line=fexp.readline() #this line is bashscript header
line=fexp.readline() #this line is expName
expName=line[8:-1]
line=fexp.readline() #this line is daqDataPath
daqDataPath=line[12:-1]
line=fexp.readline() #this line is LastRunNum
runNum=line[11:-1]
fexp.close()

if len(sys.argv) >= 2 :
    runNum=sys.argv[1]

### myEndpoint_id is in dgs, no access right to /home/helios!!!!
myEndpoint_id='d907d6b0-986e-11e9-bf5e-0e4a062367b8'
LCRCEndpoint_id='61f9954c-a4fa-11ea-8f07-0a21f750d19b'

taskLabel= format("DAQ_Auto_Upload_%s_Run%03d" % (expName, int(runNum)) )

tdata = globus_sdk.TransferData(tc, myEndpoint_id, LCRCEndpoint_id, label=taskLabel, sync_level="checksum")

'''
if int(runNum)%3!=0 : 
    print( "runNum is not mulitple of 3. sync next time.")
    exit()

source_path = format("%s/%s/data/"  % (daqDataPath, expName ))
lcrc_path = format("/lcrc/project/HELIOS/experimentsData/%s/data/" % (expName))

print( source_path)
print(lcrc_path)

tdata.add_item(source_path, lcrc_path, recursive=True)
'''

import glob

source_path = format("%s/%s/data/%s_run_%03d*"  % (daqDataPath, expName,  expName, int(runNum) ))
lcrc_path = format("/lcrc/project/HELIOS/experimentsData/%s/data" % (expName))

print( source_path)
print(lcrc_path)

for files in glob.glob(source_path):
    #print( files)
    position=files.rfind("/")
    strlen = len(files)
    filestr = files[position:strlen]
    #print( filestr)
    d1 = format("%s/%s/data%s" % (daqDataPath, expName, filestr))
    d2 = lcrc_path + filestr
    #print( d1, d2)
    tdata.add_item(d1, d2)

runTimeStamp_path= format("%s/%s/data/RunTimeStamp.dat" % (daqDataPath, expName))
runTimeStamp_path_LCRC= format("/lcrc/project/HELIOS/experimentsData/%s/data/RunTimeStamp.dat" % (expName))
tdata.add_item(runTimeStamp_path, runTimeStamp_path_LCRC)


transfer_result = tc.submit_transfer(tdata)
print("================================ submitted a job ")
print("task_id =", transfer_result["task_id"])


print("My Last 10 Tasks:")
for task in tc.task_list(num_results=10, filter="type:TRANSFER,DELETE"):
    print(task["request_time"], task["label"], task["status"])
    #print(task["task_id"], task["request_time"], task["completion_time"], task["status"])

