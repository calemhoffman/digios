#!/usr/local/bin/python2.7

import globus_sdk

#this is how to get the token.
CLIENT_ID = '1f2df281-39e0-4f75-b377-f11ae435a7e8'

client = globus_sdk.NativeAppAuthClient(CLIENT_ID)

print "==============================="
val = raw_input("create a login token (Y/n)?")

if val == 'Y' :

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

    print "refresh_token : ",transfer_at
    print "access_token  : ",transfer_rt
    print "Expired_at_secound : ",expires_at_s

    # Now we've got the data we need, but what do we do?
    # That "GlobusAuthorizer" from before is about to come to the rescue
    authorizer = globus_sdk.RefreshTokenAuthorizer(
        transfer_rt, client, access_token=transfer_at, expires_at=expires_at_s)

    # and try using `tc` to make TransferClient calls. Everything should just
    # work -- for days and days, months and months, even years
    tc = globus_sdk.TransferClient(authorizer=authorizer)


else :
    f = open("/home/helios/Globus-token.dat", "r");
    AUTH_TOKEN = f.readline().strip()
    TRANSFER_TOKEN = f.readline().strip()
    f.close()

    authorizer = globus_sdk.RefreshTokenAuthorizer(TRANSFER_TOKEN, client, access_token=AUTH_TOKEN)

    tc = globus_sdk.TransferClient(authorizer=authorizer)

    # high level interface; provides iterators for list responses
    print("Endpoints:")
    for ep in tc.endpoint_search(filter_scope="my-endpoints"):
        print("[{}] {}".format(ep["id"], ep["display_name"]))
    for ep in tc.endpoint_search('LCRC'):    
        print("[{}] {}".format(ep["id"], ep["display_name"]))
