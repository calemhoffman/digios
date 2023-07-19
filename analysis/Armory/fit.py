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

data13 = [5486.5859,
4246.0985,
3913.6398,
3558.5743,
3385.0387]

data23 = [6192.9471,
4779.4753,
4430.6912,
4016.0558,
3803.0349]

data28 =[6018.6778,
4644.2884,
4273.1485,
3872.6941,
3703.2331]

data9 = [5708.6857,
4416.5026,
4112.8463,
3704.7869,
3529.2642]

data14 =[6054.7844,
4671.1413,
4326.4956,
3921.8961,
3719.3825]

data19 = [5677.5417,
4400.4809,
4060.2774,
3668.7756,
3500.0088]

data24 =[5683.8676,
4414.3274,
4069.1193,
3707.2610,
3532.8609]

data = [5806.9907,
4495.4754,
4139.8415,
3773.0616,
3569.1381]

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
print('y = 1/%.6f*x + 1/%.4f' % (mm, bb))
fig.show()

# %%
det20a = [[0.0,6.785],[3.133,3.704],[3.533,3.275]]
df_det20a = pd.DataFrame(det20a,columns=['e','ch'])
x1 = df_det20a['ch'].to_numpy()#
y1 = df_det20a['e'].to_numpy()#
def objective(x, m, b):
	return m*x + b
popt1, _ = curve_fit(objective, x1, y1)
m, b = popt1
print('det20a = %.6f*x + %.4f' % (m, b))

det20b = [[0.0,6.723],
[0.752,5.965],
[3.133,3.650],
[3.533,3.254]]
df_det20b = pd.DataFrame(det20b,columns=['e','ch'])
x1 = df_det20b['ch'].to_numpy()#
y1 = df_det20b['e'].to_numpy()#
def objective(x, m, b):
	return m*x + b
popt1, _ = curve_fit(objective, x1, y1)
m, b = popt1
print('det20b = %.6f*x + %.4f' % (m, b))

det21a = [[0.0,7.770],
[0.752,7.047],
[3.133,4.748],
[3.533,4.377],
[4.382,3.513]]
df_det21a = pd.DataFrame(det21a,columns=['e','ch'])
x1 = df_det21a['ch'].to_numpy()#
y1 = df_det21a['e'].to_numpy()#
def objective(x, m, b):
	return m*x + b
popt1, _ = curve_fit(objective, x1, y1)
m, b = popt1
print('det21a = %.6f*x + %.4f' % (m, b))
x_line = arange(0, 10, 1)
y_line1 = objective(x_line, m, b)
fig = go.Figure()
fig.add_trace(go.Scatter(x=x_line,y=y_line1,mode='lines'))
fig.add_trace(go.Scatter(x=df_det21a['ch'],y=df_det21a['e'],
                         mode='markers'))

det21b = [[0.0,7.842],
[0.752,7.088],
[3.133,4.784],
[3.533,4.386],
[4.382,3.600]]
df_det21b = pd.DataFrame(det21b,columns=['e','ch'])
x1 = df_det21b['ch'].to_numpy()#
y1 = df_det21b['e'].to_numpy()#
def objective(x, m, b):
	return m*x + b
popt1, _ = curve_fit(objective, x1, y1)
m, b = popt1
print('det21b = %.6f*x + %.4f' % (m, b))
x_line = arange(0, 10, 1)
y_line1 = objective(x_line, m, b)
fig = go.Figure()
fig.add_trace(go.Scatter(x=x_line,y=y_line1,mode='lines'))
fig.add_trace(go.Scatter(x=df_det21b['ch'],y=df_det21b['e'],
                         mode='markers'))
det22a = [[0.752,8.055],
[3.133,5.79],
[3.533,5.407]]
df_det22a = pd.DataFrame(det22a,columns=['e','ch'])
x1 = df_det22a['ch'].to_numpy()#
y1 = df_det22a['e'].to_numpy()#
def objective(x, m, b):
	return m*x + b
popt1, _ = curve_fit(objective, x1, y1)
m, b = popt1
print('det22a = %.6f*x + %.4f' % (m, b))
x_line = arange(0, 10, 1)
y_line1 = objective(x_line, m, b)
fig = go.Figure()
fig.add_trace(go.Scatter(x=x_line,y=y_line1,mode='lines'))
fig.add_trace(go.Scatter(x=df_det22a['ch'],y=df_det22a['e'],
                         mode='markers'))
det22b = [[0.752,7.965],
[3.133,5.765],
[3.533,5.415],
[5.281,3.693]]
df_det22b = pd.DataFrame(det22b,columns=['e','ch'])
x1 = df_det22b['ch'].to_numpy()#
y1 = df_det22b['e'].to_numpy()#
def objective(x, m, b):
	return m*x + b
popt1, _ = curve_fit(objective, x1, y1)
m, b = popt1
print('det22b = %.6f*x + %.4f' % (m, b))
x_line = arange(0, 10, 1)
y_line1 = objective(x_line, m, b)
fig = go.Figure()
fig.add_trace(go.Scatter(x=x_line,y=y_line1,mode='lines'))
fig.add_trace(go.Scatter(x=df_det22b['ch'],y=df_det22b['e'],
                         mode='markers'))
det23a = [[0.752,9.148],
[3.133,6.840],
[3.533,6.44],
[5.281,4.754]]
df_det23a = pd.DataFrame(det23a,columns=['e','ch'])
x1 = df_det23a['ch'].to_numpy()#
y1 = df_det23a['e'].to_numpy()#
def objective(x, m, b):
	return m*x + b
popt1, _ = curve_fit(objective, x1, y1)
m, b = popt1
print('det23a = %.6f*x + %.4f' % (m, b))
x_line = arange(0, 11, 1)
y_line1 = objective(x_line, m, b)
fig = go.Figure()
fig.add_trace(go.Scatter(x=x_line,y=y_line1,mode='lines'))
fig.add_trace(go.Scatter(x=df_det23a['ch'],y=df_det23a['e'],
                         mode='markers'))
det23b = [[0.752,9.148],
[3.133,6.81],
[3.533,6.40],
[5.281,4.68]]
df_det23b = pd.DataFrame(det23b,columns=['e','ch'])
x1 = df_det23b['ch'].to_numpy()#
y1 = df_det23b['e'].to_numpy()#
def objective(x, m, b):
	return m*x + b
popt1, _ = curve_fit(objective, x1, y1)
m, b = popt1
print('det23b = %.6f*x + %.4f' % (m, b))
x_line = arange(0, 11, 1)
y_line1 = objective(x_line, m, b)
fig = go.Figure()
fig.add_trace(go.Scatter(x=x_line,y=y_line1,mode='lines'))
fig.add_trace(go.Scatter(x=df_det23b['ch'],y=df_det23b['e'],
                         mode='markers'))
det24a = [[3.133,7.918],
[3.533,7.526],
[4.3,6.738],
[5.281,5.812],
[7.4,3.66]]#,
#[3.02]]
df_det24a = pd.DataFrame(det24a,columns=['e','ch'])
x1 = df_det24a['ch'].to_numpy()#
y1 = df_det24a['e'].to_numpy()#
def objective(x, m, b):
	return m*x + b
popt1, _ = curve_fit(objective, x1, y1)
m, b = popt1
print('det24a = %.6f*x + %.4f' % (m, b))
x_line = arange(0, 11, 1)
y_line1 = objective(x_line, m, b)
fig = go.Figure()
fig.add_trace(go.Scatter(x=x_line,y=y_line1,mode='lines'))
fig.add_trace(go.Scatter(x=df_det24a['ch'],y=df_det24a['e'],
                         mode='markers'))
det24b = [[3.133,7.84],
[3.533,7.43],
[5.28,5.72],
[5.44,5.56],
[7.9,3.52]]#,
#[3.02]]
df_det24b = pd.DataFrame(det24b,columns=['e','ch'])
x1 = df_det24b['ch'].to_numpy()#
y1 = df_det24b['e'].to_numpy()#
def objective(x, m, b):
	return m*x + b
popt1, _ = curve_fit(objective, x1, y1)
m, b = popt1
print('det24b = %.6f*x + %.4f' % (m, b))
x_line = arange(0, 11, 1)
y_line1 = objective(x_line, m, b)
fig = go.Figure()
fig.add_trace(go.Scatter(x=x_line,y=y_line1,mode='lines'))
fig.add_trace(go.Scatter(x=df_det24b['ch'],y=df_det24b['e'],
                         mode='markers'))
# %%
def objective(x, m, b):
	return m*x + b

col0 = [0.0,0.752,2.788,3.133,3.533,4.382,None,None]
col1 = [0.0,0.752,2.788,3.133,3.533,4.382,5.281,5.44]
col27p = [0.0,0.752,None,3.133,3.533,4.261,4.382,5.281]
col3 = [None,None,None,3.133,3.533,5.281,5.44,8.0]
col3p = [0.0,0.75,3.133,3.533,5.281,5.44,6.584,8.1]
col13p = [None,3.133,3.533,4.719,5.281,5.44,6.584,8.1]
col23p = [0.0,0.75,3.133,3.533,5.281,5.44,6.584,8.1]
col28p = [0.0,0.75,3.133,3.533,4.719,5.44,6.584,8.1]
col4 = [None,None,None,3.133,3.533,5.281,5.44,9.2]
col29p = [None,3.133,3.533,4.719,5.281,5.44,6.584,9.2]

df = pd.DataFrame(col0,columns=['e0'])
df['e1'] = col1
df['e3'] = col3
df['e3p'] = col3p
df['e13p'] = col13p
df['e23p'] = col23p
df['e28p'] = col28p
df['e27p'] = col27p
df['e4'] = col4
df['e29p'] = col29p

det0 = [-0.0279,0.7561,2.7906,3.1734,3.5859,4.4157,None,None]
det15 = [0.0672,
  0.7681,
  2.8038,
  3.1699,
  3.5907,
  4.4045,
  None,
  None]
det20 = [-0.0036,
  0.7688,
  2.7630,
  3.1458,
  3.5708,
  4.3544,
 None,
 None]
det1 = [0.3990,
  0.8179,
  2.7443,
  3.1712,
  3.5802,
  4.3950,
  5.3243,
  5.45]
det6 = [0.3,
0.987,
  2.9000,
  3.2463,
  3.6500,
  4.4230,
  5.3474,
  5.5910]
det11 = [ 0.23,
  0.976,
  2.9017,
  3.2964,
  3.6844,
  4.5497,
  5.429,
  5.56]
det16 = [0.0674,
  0.7990,
  2.8351,
  3.2157,
  3.6211,
  4.4449,
  5.3923,
  5.5792]
det21 = [-0.0469,
  0.7328,
  2.7648,
  3.1016,
  3.4960,
  4.3567,
  5.2666,
  5.4412]
det26 = [-0.0876,
  0.7149,
  2.7474,
  3.0821,
  3.4709,
  4.3136,
  5.2339,
  5.4018]
det7 = [0.1684,
  0.9222,
  2.9335,
  3.2820,
  3.6692,
  4.5247,
  5.3937,
  5.5499]
det12 = [0.2121,
  0.8924,
  2.9348,
  3.2623,
  3.6574,
  4.4385,
  5.3620,
  5.5592]
det17 = [   -0.0262,
  0.8841,
  2.9000,
  3.2240,
  3.6242,
  4.4556,
  5.3891,
  5.5464]
det22 = [  0.0095,0.8104,
  2.7478,
  3.1322,
  3.5259,
  4.2979,
  5.2653,
  5.4615]
det27 = [ -0.1500, 0.6173, None, 2.977,
  3.3751,4.2261,4.37,5.2055]
#col3p
det3 = [0.031,0.922,3.2414,
  3.6626,
  5.4081,
  5.5793,
  6.72,
  8.2908]
det13 = [None,3.1443,3.5259, 4.77,5.3689,5.5458,6.678,8.1361]
det23 = [0.003,0.785,3.1463,3.5492,
  5.3080,
  5.4845,
  6.619,
  8.1248]
det28 = [-0.044,0.66,3.0612,3.4724,4.65,5.34,6.4,8.1466]
det4 = [None,None,None,3.1836,
  3.6512,
  5.4000,
  5.6000,
  9.2416]
det14 = [None,None,None,3.2905,
  3.6673,
  5.4000,
  5.5997,
  9.1733]
det19 = [None,None,None,3.2162,
  3.5947,
  5.3581,
  5.5129,
  9.1875]
det24 = [None,None,None,3.0743,
  3.4956,
  5.2500,
  5.4440,
  9.1794]
det29 = [None,3.0835,
  3.5059,
  4.69,
  5.2772,
  5.4807,
  6.62,
  9.0770]
df['det0'] = det0
df['det15'] = det15
df['det20'] = det20
df['det1'] = det1
df['det6'] = det6
df['det11'] = det11
df['det16'] = det16
df['det21'] = det21
df['det26'] = det26
df['det7'] = det7
df['det12'] = det12
df['det17'] = det17
df['det22'] = det22
df['det27'] = det27
df['det3'] = det3
df['det13'] = det13
df['det23'] = det23
df['det28'] = det28
df['det4'] = det4
df['det14'] = det14
df['det19'] = det19
df['det24'] = det24
df['det29'] = det29


dets = ['det0','det15','det20']
dets = ['det1','det6','det11','det16','det21','det26']
dets = ['det7','det12','det17','det22','det27']
dets = ['det3','det13','det23','det28']
#dets = ['det4','det14','det19','det24','det29']
dets = ['det29']
ename='e29p'

for i in range(len(dets)):
	x1 = df[dets[i]].to_numpy()#
	x1 = x1[~np.isnan(x1)]
	y1 = df[ename].to_numpy()#
	y1 = y1[~np.isnan(y1)]
	popt1, errt1 = curve_fit(objective, x1, y1)
	m, b = popt1
	mm = math.sqrt(errt1[0][0])
	bb = math.sqrt(errt1[1][1])
	print('%s = %.6f(%.6f)*x + %.4f(%.4f)' % (dets[i], m, mm, b, bb))
	print('p[%d][0] = %.6f; p[%d][1] = %.4f;' % (i,m,i,b))
	print('%d %.6f %.4f' % (i,m,b))
	x_line = arange(0, 11, 1)
	y_line1 = objective(x_line, m, b)
	fig = go.Figure()
	fig.add_trace(go.Scatter(x=x_line,y=y_line1,mode='lines'))
	fig.add_trace(go.Scatter(x=df[dets[i]],y=df[ename],
                         mode='markers'))
	fig.show()
# %%
