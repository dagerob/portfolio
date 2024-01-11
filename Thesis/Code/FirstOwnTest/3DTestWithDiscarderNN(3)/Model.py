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
DATA_SETS = 7           # How many sets of data are found
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

def generate_noisy_shape(shape):
    noisyShape = []
    for val in shape:
        noisyShape.append(val + random.uniform(-0.3, 0.3))
    return np.asarray(noisyShape)

def generate_other_shapes(data):
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
        
        # add either a mixed shape, or a noisy shape chosen randomly
        mixedShapes.append(generate_mixed_shape(data[shapeNumber1][i], data[shapeNumber2][i], data[shapeNumber3][i], data[shapeNumber4][i]))
    return np.asarray(mixedShapes)

def generate_noisy_shapes(data):
    noisyShapes = []
    for i in range(0, DATA_PER_FILE):
        shapeNumber1 = random.randint(0,len(data) - 1)
        noisyShapes.append(generate_noisy_shape(data[shapeNumber1][i]))
    return np.asarray(noisyShapes)

# set the seed so that the outcome of the data generation is always the same
random.seed(RANDOM_SEED)

# read the data
circleX = reader.ReadData("circle500LocalPos.txt")      
lineX = reader.ReadData("lighting500LocalPos.txt")      
triangleX = reader.ReadData("triangle500LocalPos.txt")  
starX = reader.ReadData("star500LocalPos.txt")          


readData = [circleX, lineX, triangleX, starX]
for data in readData:
    print(len(data))

# get the "other" data
realOtherX = reader.ReadData("other500LocalPos.txt")
print(len(realOtherX))
mixedX = generate_other_shapes(readData)  
noisyX = generate_noisy_shapes(readData)

otherData = [mixedX, noisyX, realOtherX]
# mix the data
X = [None] * (DATA_PER_FILE * DATA_SETS)
y = [None] * (DATA_PER_FILE * DATA_SETS)
for i in range(0, DATA_PER_FILE):
    # store the "other" data
    for j in range(0, len(otherData)):
        X[i * DATA_SETS + j] = otherData[j][i]
        y[i * DATA_SETS + j] = 0

    # we iterate over the shapes that were read
    for j in range(len(otherData),len(otherData) + len(readData)):
        X[i * DATA_SETS + j] = readData[j - len(otherData)][i]
        y[i * DATA_SETS + j] = 1

# convert them to numpy arrays
X = np.asarray(X)
y = np.asarray(y)

# get the train data
n_train = int(DATA_PER_FILE * DATA_SETS * TRAIN_PERCENTAGE)
trainX, testX = X[:n_train, :], X[n_train:, :]
trainy, testy = y[:n_train], y[n_train:]

# define model
model = Sequential()
model.add(Dense(60, input_dim=60, activation='relu', kernel_initializer='he_uniform'))
model.add(Dense(60, activation='relu', kernel_initializer='he_uniform'))
model.add(Dense(1, activation='sigmoid'))
opt = SGD(lr=0.01, momentum=0.9)
model.compile(loss='binary_crossentropy', optimizer=opt, metrics=['accuracy'])
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

# get the amount of the real other (the ones recorded) that are either correctly classified as other,
# or the NN sets a value less than 0.95, meaning its not absolutely sure of them bieng from that category
othersFound = 0
for data in realOtherX:
    print(othersFound, end="\r")
    thisRes = model.predict(np.array(data,ndmin=2))[0]
    if(thisRes <= 0.90):
        othersFound += 1
print("\nother shapes correctly recognized: %i\n" % (othersFound))

# test how many real shapes are mislabeled as other
setIndex = -1
for set in readData:
    setIndex += 1
    thisCatCount = 0
    print("category: %i" % (setIndex))
    for data in set:
        print(thisCatCount, end="\r")
        thisRes = model.predict(np.array(data,ndmin=2))[0]
        if thisRes[0] <= 0.90:
            thisCatCount += 1
    print("cat %i was missidentified as other %i times" % (setIndex, thisCatCount))


# save onnx file containing trained network
export_to_onnx(model)
