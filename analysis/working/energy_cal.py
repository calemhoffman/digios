#%%
from typing import Pattern
import plotly.express as px
import plotly.graph_objects as go
from plotly.graph_objs import Scatter
from plotly.subplots import make_subplots
import matplotlib.pyplot as plt
import numpy as np
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
#print(cleo(-104.5,0))

# %%
df_data = pd.read_csv("evz_data.csv")
df_data

# %% calculate score from data to lines
# get single point from data, get diff to each calc line, find index of minimum, use that value as part of sum (absolute sum)
# perhpas should use a weight based on proximity to the line??
m = 1.00
b = 0.00
stateID = 0
diff = 1000.
diffSum = 0
diffSumStart = 130.1528485495266
df_data['eprime'] = df_data['e']*m + b
print(df_data)
for i in range(df_data.shape[0]): #all points
    diff = 1000.
    for index in range(6): #all possible states
        temp = abs(df_data['eprime'].loc[i] - cleo(df_data['z'].loc[i],index))
        # print("{} {}".format(index,temp))
        if temp < diff:
            diff = temp
            stateID = index
    # print("{} {}".format(stateID,diff))
    diffSum += diff
if diffSum < diffSumStart:
    diffSumStart = diffSum
print(diffSum)
        
#%%
fig = go.Figure()
fig.add_trace(go.Scatter(x=df_data['z'],y=df_data['e'],mode='markers'))
fig.add_trace(go.Scatter(x=df_data['z'],y=df_data['eprime'],mode='markers'))
for i in range(len(states)):
    fig.add_trace(go.Scatter(x=df_cleo['z'],y=df_cleo[states[i]],mode='lines'))
fig.update_xaxes(range=[-470,-400])
fig.update_yaxes(range=[2,8])
fig.show()

# %%
