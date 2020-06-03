from ROOT import gROOT, gStyle, TCanvas, TMath, TF1, TH1F, TH2F, TFile, TTree, TChain, TObjArray, TCutG;
import numpy as np
import os
import matplotlib.pyplot as plt
import time
from array import array

gROOT.Reset()

gStyle.SetOptStat("neiou")

tree = TChain("tree")
tree.Add("A_trace_run055-057_059-085_087-099.root")
#tree.Add("A_trace_run101-144_147-165_167-214.root")
totEntry = tree.GetEntries()
print ("total Entry = ", totEntry)

x_data = np.empty((0,5))
w_data = np.empty((0,3)) # detID, coinTime, Ex

rdtScale=[1.1, 0.54, 1., 1., 1., 1., 1.05, 0.50]

#rdt1 = TH2F("rdt1", "", 300, 0, 4000, 300, 0, 6000)
#rdt2 = TH2F("rdt2", "", 300, 0, 4000, 300, 0, 6000)
#rdt3 = TH2F("rdt3", "", 300, 0, 4000, 300, 0, 6000)
#rdt4 = TH2F("rdt4", "", 300, 0, 4000, 300, 0, 6000)

print("============= read tree")
start = time.time()
tenPrecent = (int) (totEntry * 0.1)
count = 0 
for evt in tree:
  count += 1
  end = time.time()
  if count%tenPrecent == 0 : print( "count : %8d (%4.1f%%), %5.1f sec" % (count, count/totEntry*100., end - start))
  if evt.multiHit != 1 : continue
  if evt.rdtdEMultiHit != 1 : continue
  
  for i in range(len(evt.e)) :
    if TMath.IsNaN(evt.e[i]) : continue
    e_n = evt.e[i]
    z_n = evt.z[i]
    
  for i in range(len(evt.trdt)) :
    if i%2 == 1 : continue
    if TMath.IsNaN(evt.trdt[i]) : continue
    rdtE  = rdtScale[i]*evt.trdt[i]
    rdtdE = rdtScale[i+1] * evt.trdt[i+1] 
    
    #if int(i/2) == 0 : rdt1.Fill( rdtE, rdtdE) 
    #if int(i/2) == 1 : rdt2.Fill( rdtE, rdtdE) 
    #if int(i/2) == 2 : rdt3.Fill( rdtE, rdtdE) 
    #if int(i/2) == 3 : rdt4.Fill( rdtE, rdtdE) 
  
  x_data = np.append(x_data, np.array([[e_n, z_n, evt.coinTime, rdtdE, rdtE]]), axis=0)
  w_data = np.append(w_data, np.array([[evt.detID, evt.coinTime, evt.Ex]]), axis=0)
    
print("      number of count = ", count) 

print(len(x_data))
print(x_data)

#c1 = TCanvas( 'c1', 'Example with Formula', 800, 800 )
#c1.Divide(2,2)
#
#c1.cd(1); rdt1.Draw("colz")
#c1.cd(2); rdt2.Draw("colz")
#c1.cd(3); rdt3.Draw("colz")
#c1.cd(4); rdt4.Draw("colz")


print("======================== apply NN model")

import tensorflow as tf
from tensorflow import keras
print("Tensorflow version : ", tf.version.VERSION)

# Define a simple sequential model

model = tf.keras.models.Sequential([
  keras.layers.Dense(5, activation='relu'),
  keras.layers.Dropout(0.2),
  #keras.layers.Dense(10),
  keras.layers.Dense(2)
])

model.compile(optimizer='adam',
              loss=tf.losses.SparseCategoricalCrossentropy(from_logits=True),
              metrics=['accuracy'])
              
# convert to probability
probability_model = tf.keras.Sequential([
  model,
  tf.keras.layers.Softmax()
])


checkpoint_path = "training_1/cp.ckpt"
model.load_weights(checkpoint_path)


print(probability_model(np.array([x_data[1]])))

print("======================= save to a tree")
# write new tree
fOut = TFile('nnResult.root', 'recreate')
treeOut = TTree ( 'tree', 'tree test')

e_p = np.array([1], dtype=np.float32) 
z_p = np.array([1], dtype=np.float32) 
coinTime_p = np.array([1], dtype=np.float32) 
Ex_p = np.array([1], dtype=np.float32) 
rdtdE_p = np.array([1], dtype=np.float32) 
rdtE_p  = np.array([1], dtype=np.float32) 
prob_p  = np.array([1], dtype=np.float32) 

treeOut.Branch( 'e', e_p, 'e/F')
treeOut.Branch( 'z', z_p, 'z/F')
treeOut.Branch( 'coinTime', coinTime_p, 'coinTime/F')
treeOut.Branch( 'Ex', Ex_p, 'Ex/F')
treeOut.Branch( 'rdtE', rdtE_p, 'rdtE/F')
treeOut.Branch( 'rdtdE', rdtdE_p, 'rdtdE/F')
treeOut.Branch( 'prob', prob_p, 'prob/F')

start = time.time()
tenPrecent = int(len(x_data) * 0.1)
count = 0 
for i in range(len(x_data)):
  e_p[0], z_p[0], coinTime_p[0], rdtdE_p[0], rdtE_p[0] = x_data[i]
  dum, dum, Ex_p[0] = w_data[i]
  prob_p[0] = probability_model(np.array([x_data[i]])).numpy()[0,1]
  treeOut.Fill()
  count += 1
  end = time.time()
  if count%tenPrecent == 0 : print( "count : %8d (%4.1f%%), %5.1f sec" % (count, count/len(x_data)*100., end - start))
  
 
fOut.Write()
fOut.Close()

print("==============================")
t = TChain("tree")
t.Add("nnResult.root")

c1=TCanvas("c1", "c1", 800, 800)
c1.Divide(2,2)

c1.cd(1); t.Draw("prob>>h1(500, -0.1, 1.1)" )
c1.cd(2); t.Draw("e:z>>h2(500, 700, 1000, 500, 0, 20)", "prob > 0.99", "colz")
c1.cd(3); t.Draw("coinTime:Ex>>h3(500, -3, 4, 500, -50, 100)", "prob > 0.99", "colz")
c1.cd(4); t.Draw("rdtdE:rdtE>>h4(500, 0, 4000, 500, 0, 6000)", "prob > 0.99", "colz")


