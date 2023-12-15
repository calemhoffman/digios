#%%
from typing import Pattern
import plotly.express as px
import plotly.graph_objects as go
from plotly.graph_objs import Scatter
from plotly.subplots import make_subplots
import matplotlib.pyplot as plt
import numpy as np
from numpy import arange
import pandas as pd
import plotly.io as pio
import math
from scipy import special
from scipy.interpolate import splrep, splev
from scipy.optimize import curve_fit
from scipy.stats import chisquare
#color = px.colors.sequential.Plotly3
color = ['#31AFDB','#7F599A','#2AA590','#DB84B9','#A1A4A7','#E48E54','#DFBA23','#AD9B8F']
#color = ['#E97371','#2C6A6A','#865740','#F99183','#44938E','#70808F','#B93E36','#0E3A33','#B87843']
#color = ['#6B313F','#C7B486','#555555','#000000','#6B313F','#C7B486','#555555','#000000']
color = ['#025782','#C67B21','#72635C','#7C2A1C','#94592F','#64675C','#018780','#000000']
colorgrad = ['#e97371','#b5706f','#816e6d']#,'#4e6b6b','#2c6a6a']
pio.templates["mycolor"] = go.layout.Template(
    layout_colorway=color)
pio.templates.default = "mycolor"

#%%

df_cleo = pd.read_csv("cleo_si30dp_320MeV.csv")
states = ['0','1','2','3','4','5','6']
states = ['0','1','5','6','7','9']
statesint = [0,1,5,6]
df_cleo =df_cleo[['z','0','1','5','6','7','9']]
df_cleo
#%%
num_cleo = df_cleo.shape[1] - 1
tck = []
for i in range(len(states)):
    x = df_cleo['z'].to_numpy()
    y = df_cleo[states[i]].to_numpy()
    tck.append(splrep(x, y, s=0))
    
def cleo(x,stateID):
	return splev(x, tck[stateID], der=0)

df_data = pd.read_csv("evz_data.csv")
df_data

# %% calculate score from data to lines
# get single point from data, get diff to each calc line, find index of minimum, use that value as part of sum (absolute sum)
# perhpas should use a weight based on proximity to the line??
m = 1.00
b = 0.00
c = 0.00
ms = []
bs = []
cs = []
diffs = []
it = []
stateID = 0.
counterID = [0,0,0,0,0,0]
counterDiff = [0.,0.,0.,0.,0.,0.]
diff = 0.
diffSum = 0
diffSumStart = 0.

mlist = arange(0.990, 1.000, 0.0005)
blist = arange(0.1,0.16,0.002)
clist = arange(-0.001,0.001,0.0002)
# mlist = [1.0]
# blist = [0.0]
print(mlist,blist)

#%%
iter=0
for mi in mlist:
     for bi in blist:
        diffSum=0
        ave=0.
        iter=iter+1
        stateID = -1
        ms.append(mi)
        bs.append(bi)
        it.append(iter)
        df_data['eprime'] = df_data['e']*mi + bi
        #print(df_data)
        for i in range(df_data.shape[0]): #all points
            diff = 0.
            for index in range(len(states)): #all possible states
                temp = 1./(abs(df_data['eprime'].loc[i] - cleo(df_data['z'].loc[i],index)))
                # print("{} {}".format(index,temp))
                if temp > diff:
                    diff = temp
                    stateID = index
            # print("{} {}".format(stateID,diff))
            diffSum += diff
            counterDiff[stateID] = counterDiff[stateID] + diff
            counterID[stateID] = counterID[stateID] + 1
        # diffs.append(diffSum/df_data.shape[0])
        for ii in range(4):
            ave += counterDiff[ii]/counterID[ii]
        diffs.append(diffSum)
        # print(diffSum/df_data.shape[0])

#%%
fig = go.Figure()
fig = go.Figure(data =
     go.Contour(x = ms, y = bs, z = diffs))
fig.show()
        
#%%
m = 0.9975
b = 0.114
df_data['eprime'] = df_data['e']*m + b
fig = go.Figure()
fig.add_trace(go.Scatter(x=df_data['z'],y=df_data['e'],mode='markers'))
fig.add_trace(go.Scatter(x=df_data['z'],y=df_data['eprime'],mode='markers'))
for i in range(len(states)):
    fig.add_trace(go.Scatter(x=df_cleo['z'],y=df_cleo[states[i]],mode='lines'))
fig.update_xaxes(range=[-470,-120])
fig.update_yaxes(range=[1,10])
fig.update_layout(height=800,width=1200)
fig.show()

# %%
