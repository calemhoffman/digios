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
color = ['#E97371','#2C6A6A','#865740','#70808F','#B93E36','#F99183','#44938E','#B87843','#0E3A33']
colorgrad = ['#e97371','#b5706f','#816e6d']
pio.templates["mycolor"] = go.layout.Template(layout_colorway=color)
pio.templates.default = "mycolor"
stateID=0
stateID_1=0
stateID_2=0
def spfit(x, fitc2s):
	return fitc2s*splev(x, tck[stateID], der=0)
def spfit2(x, fitc2s1,fitc2s2):
     return fitc2s1*splev(x, tck[stateID_1], der=0) + fitc2s2*splev(x, tck[stateID_2], der=0)
fig = go.Figure()
for i in range(len(color)):
    fig.add_trace(go.Scatter(x=[i],y=[i],marker_color=color[i],marker_size=20))
fig.show()
# %%
data_file_name='angdist_feb7b.csv'
df_data = pd.read_csv(data_file_name,sep=',')
names = df_data.columns
print(len(names))
df_data

dwba_file_name='si30dp306MeV.Xsec.txt'
df_dwba = pd.read_fwf(dwba_file_name,header=None,skiprows=1)
df_dwba = df_dwba[:][:180]
num_dwba = df_dwba.shape[1] - 1
df_dwba
tck = []
for i in range(num_dwba):
    x = df_dwba[0].to_numpy()
    y = df_dwba[i+1].to_numpy()
    tck.append(splrep(x, y, s=0))
# %%
numex = 4
c2s = [2.,1.,1.5,2.,4.]
dwbanum = [1,6,6,6,7]
ymin=0.1
ymax=2.2
fig = make_subplots(rows=5, cols=1, shared_xaxes=True, shared_yaxes=True, 
                    horizontal_spacing=0.0,vertical_spacing=0.05)
for i in range(numex):
    fig.add_trace(go.Scatter(x = df_dwba[0],y=c2s[i]*df_dwba[dwbanum[i]]),row=i+1,col=1)
    fig.add_trace(go.Scatter(x = df_data[names[2*i]], y = df_data[names[2*i+1]],
            name=names[2*i+1],text=df_data[names[2*i+1]],error_y=dict(type='data', array=np.sqrt(df_data[names[2*i+1]])),
            mode='markers',marker_color=color[i],marker_size=10),
            row=i+1,col=1)
fig.update_layout(width=700,height=1600,showlegend=False,margin=dict(t=100,b=100,l=150,r=20),
                  font=dict(size=18, family='times'))
fig.update_yaxes(title="<i>d&#963;/d&#x3A9;</i> [arb. units]",range=[ymin,ymax],mirror=True,ticks="inside",
                 tickson="boundaries",ticklen=10,showline=True,zeroline=True,type="log",
                ticktext=['10<sup>0 </sup> ','10<sup>1 </sup> ','10<sup>2 </sup> ','10<sup>3 </sup> ','10<sup>4 </sup> '],
                tickvals=[1,10,100,1000,10000],showgrid=False)
fig.update_xaxes(title="",range=[10,50],mirror=True,ticks="inside",
                 tickson="boundaries",ticklen=10,showline=True,zeroline=True,showgrid=False,row=1,col=1)
fig.update_xaxes(title="",range=[10,50],mirror=True,ticks="inside",
                 tickson="boundaries",ticklen=10,showline=True,zeroline=True,showgrid=False,row=2,col=1)
fig.update_xaxes(title="",range=[10,50],mirror=True,ticks="inside",
                 tickson="boundaries",ticklen=10,showline=True,zeroline=True,showgrid=False,row=3,col=1)
fig.update_xaxes(title="",range=[10,50],mirror=True,ticks="inside",
                 tickson="boundaries",ticklen=10,showline=True,zeroline=True,showgrid=False,row=4,col=1)
fig.update_xaxes(title="&#x1D703;<sub><i>cm</i></sub> [deg.]",range=[10,50],mirror=True,ticks="inside",
                 tickson="boundaries",ticklen=10,showline=True,zeroline=True,showgrid=False,row=5,col=1)
fig.show()
# %%

# %%
