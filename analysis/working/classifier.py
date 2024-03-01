#%%
import pandas as pd

# Replace 'your_file.csv' with the path to your CSV file
df = pd.read_csv('all_data.csv')
print(df.size)
df = df.dropna()
print(df.size)
df
import plotly.graph_objects as go

# Create a scatter plot with Plotly graph objects
fig = go.Figure(data=go.Scatter(x=df['rdte'], y=df['Ex'], mode='markers',marker_color=df['target']))
fig.update_layout(title='Ex vs rdte', xaxis_title='rdte', yaxis_title='Ex')
fig.show()
# %%
from sklearn.model_selection import train_test_split
# df['new_column'] = np.where(df['column_name'] > 0, 0, 1)
X = df[['Ex','de','rdte']] #x,y of 2D df.drop('Ex', axis=1)  # Replace 'target_column_name' with the actual target column name
y = df['target'] #0,1,2 label

X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)
# %%
from sklearn.preprocessing import StandardScaler

scaler = StandardScaler()
X_train_scaled = scaler.fit_transform(X_train)
X_test_scaled = scaler.transform(X_test)
# %%
from sklearn.svm import SVC
from sklearn.tree import DecisionTreeClassifier
from sklearn.inspection import DecisionBoundaryDisplay

dtc = DecisionTreeClassifier(max_depth=5, random_state=42)
# svm = SVC(gamma=1,C=1, random_state=42)
dtc.fit(X_train_scaled, y_train)
score = dtc.score(X_test_scaled, y_test)
tmp = dtc.predict(X_test_scaled)
print(score)
print(y_test,tmp)

# %%
