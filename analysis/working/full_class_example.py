
#%%
#setup and load data
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from matplotlib.colors import ListedColormap

from sklearn.datasets import make_circles, make_classification, make_moons
from sklearn.discriminant_analysis import QuadraticDiscriminantAnalysis
from sklearn.ensemble import AdaBoostClassifier, RandomForestClassifier
from sklearn.gaussian_process import GaussianProcessClassifier
from sklearn.gaussian_process.kernels import RBF
from sklearn.inspection import DecisionBoundaryDisplay
from sklearn.model_selection import train_test_split
from sklearn.naive_bayes import GaussianNB
from sklearn.neighbors import KNeighborsClassifier
from sklearn.neural_network import MLPClassifier
from sklearn.pipeline import make_pipeline
from sklearn.preprocessing import StandardScaler
from sklearn.svm import SVC
from sklearn.tree import DecisionTreeClassifier


df = pd.read_csv('all_data.csv')
df = df.dropna()
X_orig = df[['rdte','Ex']]#x,y of 2D
y_orig = df['target'] #0,1,2 label
df_orig = df
df = df[df['target']>0]
df=df[df['target']<3]
df = df[df['Ex']<8.0]
X = X_orig[:200]
y = y_orig[:200]
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)
scaler = StandardScaler()
X_train = scaler.fit_transform(X_train)
X_test = scaler.transform(X_test)
X_scaled = scaler.transform(X) #all data scaled
X_orig_scaled = scaler.transform(X_orig) #all data scaled
#%%
names = [
    "Nearest Neighbors",
    # "Linear SVM",
    # "RBF SVM",
    # "Gaussian Process",
    # "Decision Tree",
    # "Random Forest",
    # "Neural Net",
    # "AdaBoost",
    # "Naive Bayes",
    # "QDA",
]

classifiers = [
    KNeighborsClassifier(3),
    # SVC(kernel="linear", C=0.025, random_state=42),
    # SVC(gamma=2, C=1, random_state=42),
    # GaussianProcessClassifier(1.0 * RBF(1.0), random_state=42),
    # DecisionTreeClassifier(max_depth=5, random_state=42),
    # RandomForestClassifier(
    #     max_depth=5, n_estimators=10, max_features=1, random_state=42
    # ),
    # MLPClassifier(alpha=1, max_iter=1000, random_state=42),
    # AdaBoostClassifier(algorithm="SAMME", random_state=42),
    # GaussianNB(),
    # QuadraticDiscriminantAnalysis(),
]

#%%
figure = plt.figure(figsize=(36, 9))
i = 1
# iterate over datasets
ds_cnt = 0
# preprocess dataset, split into training and test part
# X, y = ds
# X_train, X_test, y_train, y_test = train_test_split(
#     X, y, test_size=0.4, random_state=42
# )
x_min, x_max = X_scaled[:, 0].min() - 0.5, X_scaled[:, 0].max() + 0.5
y_min, y_max = X_scaled[:, 1].min() - 0.5, X_scaled[:, 1].max() + 0.5

# just plot the dataset first
cm = plt.cm.RdBu
cm_bright = ListedColormap(["#FF0000", "#0000FF"])
ax = plt.subplot(1, len(classifiers) + 1, i)
ax.set_title("Input data")
# Plot the training points
ax.scatter(X_train[:, 0], X_train[:, 1], c=y_train, cmap=cm_bright, edgecolors="k")
# Plot the testing points
ax.scatter(
    X_test[:, 0], X_test[:, 1], c=y_test, cmap=cm_bright, alpha=0.6, edgecolors="k"
)
ax.set_xlim(x_min, x_max)
ax.set_ylim(y_min, y_max)
ax.set_xticks(())
ax.set_yticks(())
i += 1
# iterate over classifiers
for name, clf in zip(names, classifiers):
    ax = plt.subplot(1, len(classifiers) + 1, i)

    clf = make_pipeline(StandardScaler(), clf)
    print("fitting {}".format(clf))
    clf.fit(X_train, y_train)
    score = clf.score(X_test, y_test)
    print(score)
    df_orig['pred'] = clf.predict(X_orig_scaled)

    DecisionBoundaryDisplay.from_estimator(
        clf, X_scaled, cmap=cm, alpha=0.8, ax=ax, eps=0.5
    )

    # Plot the training points
    ax.scatter(
        X_train[:, 0], X_train[:, 1], c=y_train, cmap=cm_bright, edgecolors="k"
    )
    # Plot the testing points
    ax.scatter(
        X_test[:, 0],
        X_test[:, 1],
        c=y_test,
        cmap=cm_bright,
        edgecolors="k",
        alpha=0.6,
    )
    ax.set_xlim(x_min, x_max)
    ax.set_ylim(y_min, y_max)
    ax.set_xticks(())
    ax.set_yticks(())
    ax.set_title(name)
    ax.text(
        x_max - 0.3,
        y_min + 0.3,
        ("%.2f" % score).lstrip("0"),
        size=15,
        horizontalalignment="right",
    )
    i += 1
ds_cnt+=1
plt.tight_layout()
plt.show()

#%%
import plotly.graph_objects as go
from plotly.subplots import make_subplots
import plotly.io as pio

color = ['#E97371','#2C6A6A','#865740','#70808F','#B93E36','#F99183','#44938E','#B87843','#0E3A33']
colorgrad = [color[0],color[1],color[2]]
pio.templates["mycolor"] = go.layout.Template(layout_colorway=color)
pio.templates.default = "mycolor"

fig_pred = make_subplots(rows=1, cols=2)
for i in range(2):
    fig_pred.add_trace(go.Scatter(x=[9.2,9.2], y=[0, 40],
                         mode='lines', name='Mean', line=dict(color=color[6], width=3)),row=1,col=i+1)
    fig_pred.add_trace(go.Scatter(x=[8.76,8.76], y=[0, 40],
                         mode='lines', name='Mean', line=dict(color=color[7], width=3)),row=1,col=i+1)
    fig_pred.add_trace(go.Histogram(x=df_orig[df_orig['target']==(i+1)].Ex,
                                        xbins=dict(start=-1.0,
                                        end=11.0,
                                        size=0.1),marker=dict(color=color[0],opacity=0.5, line=dict(width=0))),
                       row=1,col=i+1)
    fig_pred.add_trace(go.Histogram(x=df_orig[df_orig['pred']==(i+1)].Ex,
                                        xbins=dict(start=-1.0,
                                        end=11.0,
                                        size=0.1),marker=dict(color=color[1],opacity=0.5,line=dict(width=0))),
                       row=1,col=i+1)
    fig_pred.update_xaxes(range=[2,10],row=1,col=i+1)
fig_pred.update_layout(width=1000,height=800,barmode='overlay')

fig_pred.show()
# %%
