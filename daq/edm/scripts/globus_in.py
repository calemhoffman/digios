#!/usr/local/bin/python2.7



##### THIS IS NOT WORKING, we need to send the file to MAC

exit()

import globus_sdk

#this is how to get the token.
CLIENT_ID = '1f2df281-39e0-4f75-b377-f11ae435a7e8'

client = globus_sdk.NativeAppAuthClient(CLIENT_ID)

import os

f = open("/home/helios/Globus-token.dat", "r");
AUTH_TOKEN = f.readline().strip()
TRANSFER_TOKEN = f.readline().strip()
f.close()

#print( TRANSFER_TOKEN )

#authorizer = globus_sdk.AccessTokenAuthorizer(TRANSFER_TOKEN)
authorizer = globus_sdk.RefreshTokenAuthorizer(
    TRANSFER_TOKEN, client, access_token=AUTH_TOKEN)

tc = globus_sdk.TransferClient(authorizer=authorizer)

# high level interface; provides iterators for list responses
#print("Endpoints:")
#for ep in tc.endpoint_search(filter_scope="my-endpoints"):
#    print("[{}] {}".format(ep["id"], ep["display_name"]))
#for ep in tc.endpoint_search('LCRC'):    
#    print("[{}] {}".format(ep["id"], ep["display_name"]))

### myEndpoint_id is in dgs, no access right to /home/helios!!!!
MACEndpoint_id='d907d6b0-986e-11e9-bf5e-0e4a062367b8'
LCRCEndpoint_id='57b72e31-9f22-11e8-96e1-0a6d4e044368'

tdata = globus_sdk.TransferData(tc, LCRCEndpoint_id, DAQEndpoint_id, label="DAQ_Auto_Downlaod", sync_level="checksum")

fexp=open("/home/helios/digios/expName.sh", 'r')
line=fexp.readline() #this line is bashscript header
line=fexp.readline() #this line is expName
expName=line[8:-1]
line=fexp.readline() #this line is daqDataPath
daqDataPath=line[12:-1]
line=fexp.readline() #this line is LastRunNum
runNum=line[11:-1]
fexp.close()


#if int(runNum)%3!=0 : 
#    print( "runNum is not mulitple of 3. sync next time.")
#    exit()

DAQ_path = format("%s/%s/root_data/"  % (daqDataPath, expName ))
LCRC_path = format("/lcrc/project/HELIOS/experimentsData/%s/root_data/" % (expName))

print(DAQ_path)
print(LCRC_path)

tdata.add_item(LCRC_path, DAQ_path, recursive=True)


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

'''

transfer_result = tc.submit_transfer(tdata)
print("================================ submitted a job ")
print("task_id =", transfer_result["task_id"])


print("My Last 10 Tasks:")
for task in tc.task_list(num_results=10, filter="type:TRANSFER,DELETE"):
#    print(task["task_id"], task["request_time"], task["completion_time"], task["status"])
    print(task["request_time"], task["completion_time"], task["status"])

