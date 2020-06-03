import tensorflow as tf

#download data
mnist = tf.keras.datasets.mnist

#extract train and test data set, it is numpy.ndarray
(x_train, y_train), (x_test, y_test) = mnist.load_data()
x_train, x_test = x_train / 255.0, x_test / 255.0

#define network
model = tf.keras.models.Sequential([
  tf.keras.layers.Flatten(input_shape=(28, 28)),
  tf.keras.layers.Dense(128, activation='relu'),
  tf.keras.layers.Dropout(0.2),
  tf.keras.layers.Dense(10)
])

#put the 1st training data into the raw model
predictions = model(x_train[:1]).numpy()
print(predictions)
print(tf.nn.softmax(predictions).numpy()) #to probability

# define loss function, loss function compare the y_train and predictions, this help to define the "direction" of the optimzer
loss_fn = tf.keras.losses.SparseCategoricalCrossentropy(from_logits=True)
print(loss_fn(y_train[:1], predictions).numpy())
model.compile(optimizer='adam', loss=loss_fn, metrics=['accuracy'])

#train, epochs = loop
model.fit(x_train, y_train, epochs=5)

#evaluate
model.evaluate(x_test,  y_test, verbose=2)

# convert to probability
probability_model = tf.round(tf.keras.Sequential([
  model,
  tf.keras.layers.Softmax()
]))

print(probability_model(x_test[:5]))
