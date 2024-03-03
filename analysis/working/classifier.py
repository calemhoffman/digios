#%%
import pandas as pd

# Replace 'your_file.csv' with the path to your CSV file
df = pd.read_csv('all_data.csv')
print(df.size)
df = df.dropna()
print(df.size)
# df = df[df['target']>0] 
import plotly.graph_objects as go


# %%
from sklearn.model_selection import train_test_split
# df['new_column'] = np.where(df['column_name'] > 0, 0, 1)
X = df[['Ex','de','rdte','coinTime','thetaCM']] #x,y of 2D df.drop('Ex', axis=1)  # Replace 'target_column_name' with the actual target column name
y = df['target'] #0,1,2 label

X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)
# %%
from sklearn.preprocessing import StandardScaler

scaler = StandardScaler()
X_train_scaled = scaler.fit_transform(X_train)
X_test_scaled = scaler.transform(X_test)
X_scaled = scaler.transform(X)
# %%
from sklearn.svm import SVC
from sklearn.tree import DecisionTreeClassifier
from sklearn.inspection import DecisionBoundaryDisplay

dtc = DecisionTreeClassifier(max_depth=5, random_state=42)
# svm = SVC(gamma=1,C=1, random_state=42)
dtc.fit(X_train_scaled, y_train)
score = dtc.score(X_test_scaled, y_test)
y_pred = dtc.predict(X_scaled)
df['pred'] = y_pred
print(score)
print(y_test,y_pred)

# %%
df_o = df[df['target']==2]
df_p = df[df['pred']==2]
print("o {}, p {}",df_o.size,df_p.size)

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
