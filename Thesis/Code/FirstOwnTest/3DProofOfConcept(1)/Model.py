import re
from unittest.mock import AsyncMockMixin
import numpy as np
import onnx
import tf2onnx.convert
from matplotlib import pyplot as plt
from sklearn.datasets import make_circles
from tensorflow.keras.layers import Dense
from tensorflow.keras.models import Sequential
from tensorflow.keras.optimizers import SGD
from tensorflow.keras.utils import to_categorical
import tensorflow as tf

import DataReader as reader
import Visualizer as vis
import random

DATA_PER_FILE = 500     # how many shapes are saved per file
AMOUNT_OF_SHAPES = 4    # How many categories should we train
TRAIN_PERCENTAGE = 0.5  # Which percentage of the data to use for training
RANDOM_SEED = 0         # to make the process deterministic

def export_to_onnx(model):
    onnx_model, _ = tf2onnx.convert.from_keras(model)
    onnx.save(onnx_model, 'model.onnx')

def generate_mixed_shape(shape1, shape2, shape3, shape4):
    shape = []
    shape.extend(shape1[:15].tolist())
    shape.extend(shape2[15:30].tolist())
    shape.extend(shape3[30:45].tolist())
    shape.extend(shape4[45:60].tolist())
    return np.asarray(shape)

def generate_random_shapes(data):
    mixedShapes = []
    for i in range(0, DATA_PER_FILE):
        shapeNumber1 = random.randint(0,len(data) - 1)
        shapeNumber2 = random.randint(0,len(data) - 1)
        # to make sure different shapes are always taken
        while shapeNumber1 == shapeNumber2:
            shapeNumber2 = random.randint(0,len(data) - 1)
        shapeNumber3 = random.randint(0,len(data) - 1)
        # to make sure different shapes are always taken
        while shapeNumber2 == shapeNumber3:
            shapeNumber3 =  random.randint(0, len(data) - 1)
        shapeNumber4 = random.randint(0,len(data) - 1)
        # to make sure different shapes are always taken
        while shapeNumber4 == shapeNumber3:
            shapeNumber4 =  random.randint(0, len(data) - 1)
        mixedShapes.append(generate_mixed_shape(data[shapeNumber1][i], data[shapeNumber2][i], data[shapeNumber3][i], data[shapeNumber4][i]))
    return np.asarray(mixedShapes)

random.seed(RANDOM_SEED)

# read the data
circleX = reader.ReadData("circle500LocalPos.txt")      # cat 0
lineX = reader.ReadData("lighting500LocalPos.txt")      # cat 1
triangleX = reader.ReadData("triangle500LocalPos.txt")  # cat 2
starX = reader.ReadData("star500LocalPos.txt")          # cat 3

readData = [circleX, lineX, triangleX, starX]
for data in readData:
    print(len(data))

# mix the data
X = [None] * (DATA_PER_FILE * AMOUNT_OF_SHAPES)
y = [None] * (DATA_PER_FILE * AMOUNT_OF_SHAPES)
for i in range(0, DATA_PER_FILE):
    # we iterate over the shapes that were read
    for j in range(0, AMOUNT_OF_SHAPES):
        X[i * AMOUNT_OF_SHAPES + j] = readData[j - 1][i]
        y[i * AMOUNT_OF_SHAPES + j] = j

# convert them to numpy arrays
X = np.asarray(X)
y = np.asarray(y)

# we need to onw hot encode the data (diagonal matrix instead of integers)
y = to_categorical(y)

# split into train and test
n_train = int(DATA_PER_FILE * AMOUNT_OF_SHAPES * TRAIN_PERCENTAGE)
trainX, testX = X[:n_train, :], X[n_train:, :]
trainy, testy = y[:n_train], y[n_train:]

# define model
model = Sequential()
model.add(Dense(20, input_dim=60, activation='relu', kernel_initializer='he_uniform'))
model.add(Dense(AMOUNT_OF_SHAPES, activation='softmax'))
opt = SGD(lr=0.01, momentum=0.9)
model.compile(loss='categorical_crossentropy', optimizer=opt, metrics=['accuracy'])
# print the model
vis.visualize_nn(model)
# fit model
history = model.fit(trainX, trainy, validation_data=(testX, testy), epochs=1000, verbose=0)
# evaluate the model
_, train_acc = model.evaluate(trainX, trainy, verbose=0)
_, test_acc = model.evaluate(testX, testy, verbose=0)
print('\nTrain: %.3f, Test: %.3f' % (train_acc, test_acc))
# plot loss during training
plt.subplot(211)
plt.title('Loss')
plt.plot(history.history['loss'], label='train')
plt.plot(history.history['val_loss'], label='test')
plt.legend()
# plot accuracy during training
plt.subplot(212)
plt.title('Accuracy')
plt.plot(history.history['accuracy'], label='train')
plt.plot(history.history['val_accuracy'], label='test')
plt.legend()
plt.show()

# save onnx file containing trained network
export_to_onnx(model)
