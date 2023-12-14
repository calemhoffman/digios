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
#%%
fig = go.Figure()
fig.add_trace(go.Scatter(x=df_data['z'],y=df_data['e'],mode='markers'))
for i in range(len(states)):
    fig.add_trace(go.Scatter(x=df_cleo['z'],y=df_cleo[states[i]],mode='lines'))
fig.update_xaxes(range=[-470,-400])
fig.update_yaxes(range=[2,8])
fig.show()
# %%

