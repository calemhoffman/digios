#!/usr/local/bin/python2.7

import globus_sdk

#this is how to get the token.
CLIENT_ID = '1f2df281-39e0-4f75-b377-f11ae435a7e8'

client = globus_sdk.NativeAppAuthClient(CLIENT_ID)

'''
client.oauth2_start_flow(refresh_tokens=True)

authorize_url = client.oauth2_get_authorize_url()
print('Please go to this URL and login: {0}'.format(authorize_url))

# this is to work on Python2 and Python3 -- you can just use raw_input() or
# input() for your specific version
get_input = getattr(__builtins__, 'raw_input', input)
auth_code = get_input('Please enter the code you get after login here: ').strip()
token_response = client.oauth2_exchange_code_for_tokens(auth_code)

print( str(token_response.by_resource_server))

globus_auth_data = token_response.by_resource_server['auth.globus.org']
globus_transfer_data = token_response.by_resource_server['transfer.api.globus.org']

# most specifically, you want these tokens as strings
AUTH_TOKEN = globus_auth_data['access_token']
TRANSFER_TOKEN = globus_transfer_data['access_token']

print( AUTH_TOKEN)
print( TRANSFER_TOKEN )

# a GlobusAuthorizer is an auxiliary object we use to wrap the token. In
# more advanced scenarios, other types of GlobusAuthorizers give us
# expressive power
authorizer = globus_sdk.AccessTokenAuthorizer(TRANSFER_TOKEN)
tc = globus_sdk.TransferClient(authorizer=authorizer)

# high level interface; provides iterators for list responses
print("My Endpoints:")
for ep in tc.endpoint_search(filter_scope="my-endpoints"):
    print("[{}] {}".format(ep["id"], ep["display_name"]))

# let's get stuff for the Globus Transfer service
globus_transfer_data = token_response.by_resource_server['transfer.api.globus.org']
# the refresh token and access token, often abbr. as RT and AT
transfer_rt = globus_transfer_data['refresh_token']
transfer_at = globus_transfer_data['access_token']
expires_at_s = globus_transfer_data['expires_at_seconds']

print( transfer_at )
print( transfer_rt)
print( expires_at_s)

# Now we've got the data we need, but what do we do?
# That "GlobusAuthorizer" from before is about to come to the rescue
authorizer = globus_sdk.RefreshTokenAuthorizer(
    transfer_rt, client, access_token=transfer_at, expires_at=expires_at_s)

# and try using `tc` to make TransferClient calls. Everything should just
# work -- for days and days, months and months, even years
tc = globus_sdk.TransferClient(authorizer=authorizer)

'''

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


fexp=open("/home/helios/digios/expName.sh", 'r')
line=fexp.readline() #this line is bashscript header
line=fexp.readline() #this line is expName
expName=line[8:-1]
line=fexp.readline() #this line is daqDataPath
daqDataPath=line[12:-1]
line=fexp.readline() #this line is LastRunNum
runNum=line[11:-1]
fexp.close()

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


