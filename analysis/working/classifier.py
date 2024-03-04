#%%
import pandas as pd
import plotly.graph_objects as go
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
colorgrad = [color[0],color[1],color[2]]
pio.templates["mycolor"] = go.layout.Template(layout_colorway=color)
pio.templates.default = "mycolor"

from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler

#%% 
#read in and plot data
df = pd.read_csv('all_data.csv')
df = df.dropna()
print(df)
df_orig = df
# df = df[df['target']>0] 
df = df[df['coinTime']>-18]
df = df[df['coinTime']<18]
df = df[df['x']<0.98]
df = df[df['x']>-0.98]
# df = df[df['rdte']<3900]
# df = df[df['rdte']>3750]
#plot originals
fig_orig = make_subplots(rows=2, cols=1)

fig_orig.add_trace(go.Histogram(x=df_orig[df_orig['target']==2].Ex,
                                       xbins=dict(start=-1.0,
                                                  end=9.0,
                                                  size=0.05)),row=1,col=1)
fig_orig.add_trace(go.Histogram(x=df[df['target']==2].Ex,
                                       xbins=dict(start=-1.0,
                                                  end=9.0,
                                                  size=0.05)),row=1,col=1)
fig_orig.update_xaxes(range=[4,9],row=1,col=1)

fig_orig.add_trace(go.Scatter(x=df.rdte,y=df.Ex,
                                     mode='markers',marker_color=df.target),row=2,col=1)
fig_orig.update_xaxes(range=[2700,4100],row=2,col=1)

fig_orig.update_layout(width=600,height=800,barmode='overlay')
fig_orig.show()

# %%
# prepare data for learning / training
# df['new_column'] = np.where(df['column_name'] > 0, 0, 1) use if One V All
df = df[df['target']>=0]
X = df[['Ex','de','rdte','coinTime','thetaCM']] #x,y of 2D df.drop('Ex', axis=1)  # Replace 'target_column_name' with the actual target column name
y = df['target'] #0,1,2 label
X_orig = df_orig[['Ex','de','rdte','coinTime','thetaCM']]

X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)
scaler = StandardScaler()
X_train_scaled = scaler.fit_transform(X_train)
X_test_scaled = scaler.transform(X_test)
X_scaled = scaler.transform(X) #all data scaled
X_orig_scaled = scaler.transform(X_orig) #all data scaled

#%%
#Decision Tree Classifier [dtc]
from sklearn.tree import DecisionTreeClassifier
from sklearn.inspection import DecisionBoundaryDisplay

dtc = DecisionTreeClassifier(max_depth=5, random_state=42)#set
dtc.fit(X_train_scaled, y_train)#fit
score = dtc.score(X_test_scaled, y_test)#score
print(score)
y_pred = dtc.predict(X_scaled)#predict
y_orig_pred = dtc.predict(X_orig_scaled)#predict
df_orig['pred'] = y_orig_pred#append to full df (make sure df is not modified here)

#%%
# Other Classifier

#%%
#Other Classifier


# %%
#numerical evaluations
df = df_orig
for type in range(4):
    df_o = df[df['target']==type]
    df_p = df[df['pred']==type]
    print("Type: {}, Orig: {}, Pred: {}".format(type,df_o.size,df_p.size))

#%%
fig_orig = make_subplots(rows=2, cols=1)

fig_orig.add_trace(go.Histogram(x=df_orig[df_orig['target']==2].Ex,
                                       xbins=dict(start=-1.0,
                                                  end=9.0,
                                                  size=0.05)),row=1,col=1)
fig_orig.add_trace(go.Histogram(x=df[df['target']==2].Ex,
                                       xbins=dict(start=-1.0,
                                                  end=9.0,
                                                  size=0.05)),row=1,col=1)
fig_orig.update_xaxes(range=[4,9],row=1,col=1)

fig_orig.add_trace(go.Scatter(x=df.rdte,y=df.Ex,
                                     mode='markers',marker_color=df.target),row=2,col=1)
fig_orig.update_xaxes(range=[2700,4100],row=2,col=1)

fig_orig.update_layout(width=600,height=800,barmode='overlay')
fig_orig.show()

#%%
fig = go.Figure(data=go.Histogram(x=df_o['Ex'],xbins=dict(start=-0.5, end=10.5, size=11./210.)))
fig.show()

fig = go.Figure()
fig.add_trace(go.Histogram(x=df_p['Ex'],xbins=dict(start=-0.5, end=10.5, size=11./210.)))
fig.show()

# Create a scatter plot with Plotly graph objects
fig = go.Figure(data=go.Scatter(x=df['rdte'], y=df['Ex'], mode='markers',marker_color=df['target']))
fig.update_layout(title='Ex vs rdte', xaxis_title='rdte', yaxis_title='Ex')
fig.show()

# Create a scatter plot with Plotly graph objects
fig = go.Figure(data=go.Scatter(x=df['rdte'], y=df['Ex'], mode='markers',marker_color=df['pred']))
fig.update_layout(title='Ex vs rdte', xaxis_title='rdte', yaxis_title='Ex')
fig.show()

# Create a scatter plot with Plotly graph objects
df = df[df['rdtID']==1]
fig = go.Figure(data=go.Scatter(x=df['rdte'], y=df['de'], mode='markers',marker_color=df['pred']))
fig.update_layout(title='DE vs. E', xaxis_title='rdte', yaxis_title='Ex')
fig.show()

# %%
