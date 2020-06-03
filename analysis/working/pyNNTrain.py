from ROOT import gROOT, gStyle, TCanvas, TMath, TF1, TH1F, TH2F, TFile, TTree, TChain, TObjArray, TCutG;
import numpy as np
import os
import matplotlib.pyplot as plt

gROOT.Reset()

gStyle.SetOptStat("neiou")

tree = TChain("tree")
tree.Add("nntree.root")
totEntry = tree.GetEntries()
print ("total Entry = ", totEntry)

x_train = np.empty((0,5))
x_test = np.empty((0,5))

y_train = np.array([]);
y_test = np.array([]);

nTrain = int(totEntry * 0.6)

count = 0 
count_train = 0
for evt in tree:
  if count < nTrain :
    x_train = np.append(x_train, np.array([[evt.e, 
                                            evt.z, 
                                            evt.coinTime, 
                                            evt.rdtdE, 
                                            evt.rdtE]]), axis=0)
    y_train = np.append(y_train, evt.isGood);
    count_train += 1
  else :
    x_test = np.append(x_test, np.array([[evt.e, 
                                          evt.z, 
                                          evt.coinTime, 
                                          evt.rdtdE, 
                                          evt.rdtE]]), axis=0)
    y_test = np.append(y_test, evt.isGood);
  count += 1
    
print("      number of count = ", count) 
print("number of count train = ", count_train) 

print(x_train)

print("======================== start training")

nEpoch = 20
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

checkpoint_path = "training_1/cp.ckpt"
checkpoint_dir = os.path.dirname(checkpoint_path)

# Create a callback that saves the model's weights
cp_callback = tf.keras.callbacks.ModelCheckpoint(filepath=checkpoint_path,
                                                 save_weights_only=True,
                                                 verbose=1)

#model.load_weights(checkpoint_path)

#train, epochs = loop
history = model.fit(x_train, y_train, epochs=nEpoch, callbacks=[cp_callback])
print("======================= ")

#model.summary()

#evaluate
model.evaluate(x_test,  y_test)

# convert to probability
probability_model = tf.keras.Sequential([
  model,
  tf.keras.layers.Softmax()
])

print(probability_model(x_test[:5]))
print(y_test[:5])

fig, axs = plt.subplots(1, 2, figsize=(6, 3))

axs[0].set_xlabel('epoch')
axs[0].set_ylabel('loss')
axs[0].plot(history.history['loss'])
axs[1].set_xlabel('epoch')
axs[1].set_ylabel('accuracy')
axs[1].set_ylim(0, 1)
axs[1].plot(history.history['accuracy'])
plt.show()

