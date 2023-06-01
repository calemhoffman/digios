#%%
import uproot
import os
import pandas as pd
import plotly.express as px
import plotly.graph_objects as go
from plotly.graph_objs import Scatter
from plotly.subplots import make_subplots
import plotly.io as pio
import awkward as ak
#pio.templates
color = ['#31AFDB','#7F599A','#2AA590','#DB84B9','#A1A4A7','#E48E54','#DFBA23','#AD9B8F','#100000']#['#2CBDFE','#47DBCD','#F3A0F2','#9D2EC5','#661D98','#F5B14C',"#A1A4A7","#100000","#F8F9FA","#A1A4A7"]
pio.templates["google"] = go.layout.Template(
    layout_colorway=color)
pio.templates.default = "google"
# %% I/O
tree =  uproot.open("/Users/calemhoffman/Research/rea/digios_git/analysis/root_data/h066_31Si_trace_run025.root")["tree"]
print("TTree had ",tree.num_entries," entries")
num_entries = tree.num_entries
branches = tree.arrays(entry_start=0, entry_stop=num_entries)
print(branches[0].tolist())

#%% Recoil DE/E Calibrations
#convert rdt array into df with deID eID de e de_t e_t
de = []
e = []
deID = []
eID = []
rdt = pd.DataFrame()
for i in range(num_entries):
    array = []
    array = branches[i].rdt
    #print(array)
    for j in range(4):
         if (array[2*j]>0):
            e.append(array[2*j])
            eID.append(2*j)
            de.append(array[2*j+1])
            deID.append(2*j+1)
            #print(de)
rdt['deID'] = pd.DataFrame(deID)
rdt['de'] = pd.DataFrame(de)
rdt['eID'] = pd.DataFrame(eID)
rdt['e'] = pd.DataFrame(e)

fig  = go.Figure()
fig.add_trace(go.Scatter(x=rdt[(rdt['eID']==0)].e,y=rdt[(rdt['deID']==1)].de,mode='markers',
        line=dict(width=0.5,color=color[0]),hoverinfo='skip',
        showlegend=False))
fig.update_layout(width=700,height=600)
fig.show()

#%%
id_cut = branches["detID"] > 0
rdt_cut = ak.any(branches["rdt"] > 20, axis=1)
cut = (id_cut & rdt_cut)
final = branches[cut]
final["e"]
# %% PLOTS
fig  = go.Figure()
#fig.add_trace(go.Histogram(x=ak.flatten(branches['e'])))
fig.add_trace(go.Histogram(x=final['Ex']))
fig.update_traces(xbins=dict(start=-1,end=11.0,size=0.2))
fig.update_layout(width=700,height=600)
fig.show()

# %%
fig  = go.Figure()
#fig.add_trace(go.Histogram(x=ak.flatten(branches['e'])))
fig.add_trace(go.Scatter(x=,y=[8,8],mode='lines',
        line=dict(width=0.5,color='lightgrey'),hoverinfo='skip',
        howlegend=False))
fig.update_layout(width=700,height=600)
fig.show()

# %%
