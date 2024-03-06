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
X = df[['Ex','rdte','coinTime','x','e']] #x,y of 2D df.drop('Ex', axis=1)  # Replace 'target_column_name' with the actual target column name
y = df['target'] #0,1,2 label
X_orig = df_orig[['Ex','rdte','coinTime','x','e']]

X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)
scaler = StandardScaler()
X_train_scaled = scaler.fit_transform(X_train)
X_test_scaled = scaler.transform(X_test)
X_scaled = scaler.transform(X) #all data scaled
X_orig_scaled = scaler.transform(X_orig) #all data scaled

#%%

from sklearn.neural_network import MLPClassifier
clf = MLPClassifier(solver='lbfgs', alpha=1e-5,
                    hidden_layer_sizes=(5, 2), random_state=1,max_iter=1000)

clf.fit(X_train_scaled, y_train)
print(clf.score(X_test_scaled,y_test))



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
import matplotlib.pyplot as plt
import numpy as np

from sklearn import datasets
from sklearn.gaussian_process import GaussianProcessClassifier
from sklearn.gaussian_process.kernels import RBF

# import some data to play with
# iris = datasets.load_iris()
# X = iris.data[:, :2]  # we only take the first two features.
# y = np.array(iris.target, dtype=int)
X = X_scaled
h = 0.2  # step size in the mesh

kernel = 1.0 * RBF([1.0])
gpc_rbf_isotropic = GaussianProcessClassifier(kernel=kernel).fit(X, y)

#%%
kernel = 1.0 * RBF([1.0, 1.0])
gpc_rbf_anisotropic = GaussianProcessClassifier(kernel=kernel).fit(X, y)

# create a mesh to plot in
x_min, x_max = X[:, 0].min() - 1, X[:, 0].max() + 1
y_min, y_max = X[:, 1].min() - 1, X[:, 1].max() + 1
xx, yy = np.meshgrid(np.arange(x_min, x_max, h), np.arange(y_min, y_max, h))

titles = ["Isotropic RBF", "Anisotropic RBF"]
plt.figure(figsize=(10, 5))
#%%
for i, clf in enumerate((gpc_rbf_isotropic, gpc_rbf_anisotropic)):
    # Plot the predicted probabilities. For that, we will assign a color to
    # each point in the mesh [x_min, m_max]x[y_min, y_max].
    plt.subplot(1, 2, i + 1)

    Z = clf.predict_proba(np.c_[xx.ravel(), yy.ravel()])

    # Put the result into a color plot
    Z = Z.reshape((xx.shape[0], xx.shape[1], 3))
    plt.imshow(Z, extent=(x_min, x_max, y_min, y_max), origin="lower")

    # Plot also the training points
    plt.scatter(X[:, 0], X[:, 1], c=np.array(["r", "g", "b"])[y], edgecolors=(0, 0, 0))
    plt.xlabel("Sepal length")
    plt.ylabel("Sepal width")
    plt.xlim(xx.min(), xx.max())
    plt.ylim(yy.min(), yy.max())
    plt.xticks(())
    plt.yticks(())
    plt.title(
        "%s, LML: %.3f" % (titles[i], clf.log_marginal_likelihood(clf.kernel_.theta))
    )

plt.tight_layout()
plt.show()
# %%
# Authors: Clay Woolam   <clay@woolam.org>
#          Oliver Rausch <rauscho@ethz.ch>
# License: BSD

import matplotlib.pyplot as plt
import numpy as np

from sklearn import datasets
from sklearn.semi_supervised import LabelSpreading, SelfTrainingClassifier
from sklearn.svm import SVC

iris = datasets.load_iris()

# X = iris.data[:, :2]
# y = iris.target
X = X_scaled
# step size in the mesh
h = 0.02

rng = np.random.RandomState(0)
y_rand = rng.rand(y.shape[0])
y_30 = np.copy(y)
y_30[y_rand < 0.3] = -1  # set random samples to be unlabeled
y_50 = np.copy(y)
y_50[y_rand < 0.5] = -1
# we create an instance of SVM and fit out data. We do not scale our
# data since we want to plot the support vectors
ls30 = (LabelSpreading().fit(X, y_30), y_30, "Label Spreading 30% data")
ls50 = (LabelSpreading().fit(X, y_50), y_50, "Label Spreading 50% data")
ls100 = (LabelSpreading().fit(X, y), y, "Label Spreading 100% data")

# the base classifier for self-training is identical to the SVC
base_classifier = SVC(kernel="rbf", gamma=0.5, probability=True)
st30 = (
    SelfTrainingClassifier(base_classifier).fit(X, y_30),
    y_30,
    "Self-training 30% data",
)
st50 = (
    SelfTrainingClassifier(base_classifier).fit(X, y_50),
    y_50,
    "Self-training 50% data",
)

rbf_svc = (SVC(kernel="rbf", gamma=0.5).fit(X, y), y, "SVC with rbf kernel")

# create a mesh to plot in
x_min, x_max = X[:, 0].min() - 1, X[:, 0].max() + 1
y_min, y_max = X[:, 1].min() - 1, X[:, 1].max() + 1
xx, yy = np.meshgrid(np.arange(x_min, x_max, h), np.arange(y_min, y_max, h))

color_map = {-1: (1, 1, 1), 0: (0, 0, 0.9), 1: (1, 0, 0), 2: (0.8, 0.6, 0)}

classifiers = (ls30, st30, ls50, st50, ls100, rbf_svc)
for i, (clf, y_train, title) in enumerate(classifiers):
    # Plot the decision boundary. For that, we will assign a color to each
    # point in the mesh [x_min, x_max]x[y_min, y_max].
    plt.subplot(3, 2, i + 1)
    Z = clf.predict(np.c_[xx.ravel(), yy.ravel()])

    # Put the result into a color plot
    Z = Z.reshape(xx.shape)
    plt.contourf(xx, yy, Z, cmap=plt.cm.Paired)
    plt.axis("off")

    # Plot also the training points
    colors = [color_map[y] for y in y_train]
    plt.scatter(X[:, 0], X[:, 1], c=colors, edgecolors="black")

    plt.title(title)

plt.suptitle("Unlabeled points are colored white", y=0.1)
plt.show()

# %%
