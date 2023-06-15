#%%
import plotly.express as px
import plotly.graph_objects as go
from plotly.graph_objs import Scatter
from plotly.subplots import make_subplots
import numpy as np
import pandas as pd
import plotly.io as pio
import math
from scipy import special
from scipy.optimize import curve_fit
from numpy import arange
# %%
data0 = [5924.4705,
4573.6279,
4238.9044,
3839.9524,
3651.1881]

data1 = [5973.2914,
4588.3330,
4257.1569,
3852.9343,
3647.0336]

data3 = [5683.3999,
4412.1491,
4090.7749,
3700.3332,
3527.9517]

data4 = [5555.7946,
4313.2110,
4001.7633,
3618.4620,
3460.7227]

data15 = [5780.6206,
4431.7120,
4105.8609,
3737.4924,
3534.2513]

data20 = [5833.7545,
4479.7225,
4160.3269,
3769.4347,
3571.1397]

data6 = [5781.3287,
4450.1046,
4120.1034,
3731.0441,
3515.1151]

data11 = [6443.5225,
4951.6392,
4601.6427,
4162.3481,
3963.7415]

data16 = [5773.5116,
4432.6895,
4123.3985,
3721.6810,
3555.0358]

data21 = [6193.5937,
4752.7956,
4414.7865,
3985.5829,
3806.8801]

data26 = [6108.8833,
4688.8106,
4345.0998,
3926.8335,
3722.6097]

data7 = [5934.4843,
4547.8282,
4234.3390,
3833.4503,
3663.8981]

data12 = [5899.9047,
4510.2328,
4195.4965,
3779.5978,
3628.2993]

data17 = [6040.0262,
4629.3572,
4279.3711,
3887.9688,
3711.8464]

data22 =[5838.9068,
4499.6380,
4167.9068,
3767.1618,
3573.1477]

data27 =[5534.6887,
4256.6031,
3951.1961,
3528.5123,
3359.3110]


data2 = [8.784,6.778,6.288,5.685,5.423]

df_efit = pd.DataFrame(data2, columns = ["e"])
df_efit["ch"] = data

df_efit
# define the true objective function
def objective(x, m, b):
	return m*x + b
x1 = df_efit['ch'].to_numpy()#
y1 = df_efit['e'].to_numpy()#
popt1, _ = curve_fit(objective, x1, y1)
m, b = popt1
print('y (etot) = %.6f*x + %.4f' % (m, b))
x_line = arange(0, 8000, 1)
y_line1 = objective(x_line, m, b)

fig = go.Figure()
fig.add_trace(go.Scatter(x=x_line,y=y_line1))
fig.add_trace(go.Scatter(x=df_efit['ch'],y=df_efit['e'],
                         mode='markers'))
mm=1./m
bb=1./b
print('y (etot) = 1/%.6f*x + 1/%.4f' % (mm, bb))
fig.show()

# %%
