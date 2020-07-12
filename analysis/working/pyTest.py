from ROOT import gROOT,TFile, TTree, TCanvas, TH2F
from array import array
import numpy as np
import os
import matplotlib.pyplot as plt
import time

n = 1000
n1 = int(n*0.3) #number of training

x_data = np.zeros([n,2])
y_data = np.zeros(n)

print("======================= generate data")

for i in range(n):
  x_data[i, 0] = (np.random.rand()-0.5)*2
  if i%3 == 0 :
    x_data[i, 1] = 4*x_data[i, 0]**2 + 0.5 + np.random.normal(0, 0.1)
    y_data[i] = 0
#  elif i%3 == 1 :
#    x_data[i, 1] = 4*x_data[i, 0]**2 + 1.1 + np.random.normal(0, 0.1)
#    y_data[i] = 1
  else:
    x_data[i, 1] = 4*x_data[i, 0]**2 + 1.1 + np.random.normal(0, 0.1)
    y_data[i] = 1

#plt.plot(x_data[:,0], x_data[:,1], 'ro')
#plt.show()


x_train = x_data[:n1]
y_train = y_data[:n1]

x_test = x_data[n1:]
y_test = y_data[n1:]


print("========================= setting up tensorflow")

nEpoch = 1000
import tensorflow as tf
from tensorflow import keras
print("Tensorflow version : ", tf.version.VERSION)

# Define a simple sequential model

model = tf.keras.models.Sequential([
  #keras.layers.Dense(2, activation='relu'),
  keras.layers.Dense(10, activation='tanh'),
  #keras.layers.Dense(10, activation='tanh'),
  #keras.layers.Dropout(0.2),
  #keras.layers.Dense(10),
  keras.layers.Dense(2)
])

model.compile(optimizer='adam',
              loss=tf.losses.SparseCategoricalCrossentropy(from_logits=True),
              metrics=['accuracy'])
#train, epochs = loop
history = model.fit(x_train, y_train, epochs=nEpoch)
print("======================= ")


#=========================================
#evaluate
model.evaluate(x_test,  y_test)

# convert to probability
probability_model = tf.keras.Sequential([
  model,
  tf.keras.layers.Softmax()
])



print("=================== make tree")

treeOut = TTree ( 'tree', 'tree test')

x = np.array([1], dtype=np.float32) 
y = np.array([1], dtype=np.float32) 
prob = np.array([1], dtype=np.float32) 
tag = np.array([1], dtype=np.int32) 

treeOut.Branch( 'x', x, 'x/F')
treeOut.Branch( 'y', y, 'y/F')
treeOut.Branch( 'prob', prob, 'prob/F')
treeOut.Branch( 'tag', tag, 'tag/I')

start = time.time()
tenPrecent = int(len(x_data) * 0.1)
count = 0 
for i in range(len(x_test)):
  x[0], y[0] = x_test[i]
  prob[0] = probability_model(np.array([x_test[i]])).numpy()[0,1]
  tag[0] = y_test[i]
  treeOut.Fill()
  count += 1
  end = time.time()
  if count%tenPrecent == 0 : print( "count : %8d (%4.1f%%), %5.1f sec" % (count, count/len(x_test)*100., end - start))
  

print("====================== Draw history")
fig, axs = plt.subplots(1, 2, figsize=(6, 3))

axs[0].set_xlabel('epoch')
axs[0].set_ylabel('loss')
axs[0].plot(history.history['loss'])
axs[1].set_xlabel('epoch')
axs[1].set_ylabel('accuracy')
axs[1].set_ylim(0, 1)
axs[1].plot(history.history['accuracy'])
plt.ion()
plt.show(block = False)


c1 = TCanvas("c1", "c1", 1200, 400)
c1.Divide(3,1)
c1.cd(1)
treeOut.Draw("y:x>>h1(200, -1, 1, 200, 0, 5)", "", "box")
c1.cd(2)
treeOut.Draw("tag:prob", "", "colz")
c1.cd(3)
treeOut.Draw("y:x:prob>>h2(200, -1, 1, 200, 0, 5)", "", "colz")


