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
AMOUNT_OF_SHAPES = 5    # How many categories should we train
TRAIN_PERCENTAGE = 0.9  # Which percentage of the data to use for training
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
        if random.randint(0,1) == 1:
            mixedShapes.append(generate_mixed_shape(data[shapeNumber1][i], data[shapeNumber2][i], data[shapeNumber3][i], data[shapeNumber4][i]))
        else:
            mixedShapes.append(generate_noisy_shape(data[shapeNumber1][i]))
    return np.asarray(mixedShapes)

# specific function to read the four txts for this experiment
def evalueate_user_data(model, file1, file2, file3, file4):
    # read the data from the other person
    userCircleX = reader.ReadData(file1)      # cat 1
    userLineX = reader.ReadData(file2)      # cat 2
    userTriangleX = reader.ReadData(file3)  # cat 3
    userStarX = reader.ReadData(file4)          # cat 4
    userReadData = [userCircleX, userLineX, userTriangleX, userStarX]
    userDrawingCount = 50

    #build the arrays
    userTestX = [None] * (userDrawingCount * (AMOUNT_OF_SHAPES - 1))
    userTesty = [None] * (userDrawingCount * (AMOUNT_OF_SHAPES - 1))
    for i in range(0, userDrawingCount):
        # we iterate over the shapes that were read
        for j in range(0, AMOUNT_OF_SHAPES - 1):
            userTestX[i * (AMOUNT_OF_SHAPES - 1) + j] = userReadData[j][i]
            userTesty[i * (AMOUNT_OF_SHAPES - 1) + j] = j + 1
    userTestX = np.asarray(userTestX)
    userTesty = np.asarray(userTesty)
    userTesty = to_categorical(userTesty)

    # evaluate the model with the user data
    _, test_acc = model.evaluate(userTestX, userTesty, verbose=0)
    print(('\n' + file1 +': Test: %.3f') % (test_acc))

    # test how many real shapes are mislabeled as other
    setIndex = -1
    for set in userReadData:
        setIndex += 1
        thisCatCount = 0
        correctCount = 0
        print("category: %i" % (setIndex))
        for data in set:
            print(thisCatCount, end="\r")
            thisRes = model.predict(np.array(data,ndmin=2))[0]
            if thisRes[0] >= 0.95:
                thisCatCount += 1
            if thisRes[setIndex + 1] >= 0.95:
                correctCount += 1
        print("cat %i was missidentified as other %i times" % (setIndex, thisCatCount))
        print("cat %i was correctly identified %i times" % (setIndex, correctCount))


# set the seed so that the outcome of the data generation is always the same
random.seed(RANDOM_SEED)

# ===========================================================================
# BUILDING DATA
# ==========================================================================

# read the data
circleX = reader.ReadData("circle500LocalPos.txt")      # cat 1
lineX = reader.ReadData("lighting500LocalPos.txt")      # cat 2
triangleX = reader.ReadData("triangle500LocalPos.txt")  # cat 3
starX = reader.ReadData("star500LocalPos.txt")          # cat 4
realOtherX = reader.ReadData("other500LocalPos.txt")


readData = [circleX, lineX, triangleX, starX]
for data in readData:
    print(len(data))
print(len(realOtherX))
mixedX = generate_other_shapes(readData)       # cat 0

# mix the data
X = [None] * (DATA_PER_FILE * AMOUNT_OF_SHAPES)
y = [None] * (DATA_PER_FILE * AMOUNT_OF_SHAPES)
for i in range(0, DATA_PER_FILE):
    # the mixed one is always the first

    # add a couple of recorded other shapes to help the algorithm understand those too
    #   aprox a third of the shapes will be these other shapes
    # if random.randint(0,2) == 1:
    #     X[i * AMOUNT_OF_SHAPES] = realOtherX[i]
    # else:
    #     X[i * AMOUNT_OF_SHAPES] = mixedX[i]

    X[i * AMOUNT_OF_SHAPES] = mixedX[i]
    y[i * AMOUNT_OF_SHAPES] = 0

    # we iterate over the shapes that were read
    for j in range(1, AMOUNT_OF_SHAPES):
        X[i * AMOUNT_OF_SHAPES + j] = readData[j - 1][i]
        y[i * AMOUNT_OF_SHAPES + j] = j

# convert them to numpy arrays
X = np.asarray(X)
y = np.asarray(y)

# we need to hot encode the data (diagonal matrix instead of integers)
y = to_categorical(y)

# get the train data
n_train = int(DATA_PER_FILE * AMOUNT_OF_SHAPES * TRAIN_PERCENTAGE)
trainX, _ = X[:n_train, :], X[n_train:, :]
trainy, _ = y[:n_train], y[n_train:]

# make a test set with the recorded other data
X = [None] * (DATA_PER_FILE * AMOUNT_OF_SHAPES)
y = [None] * (DATA_PER_FILE * AMOUNT_OF_SHAPES)
for i in range(0, DATA_PER_FILE):
    # the mixed one is always the first
    X[i * AMOUNT_OF_SHAPES] = realOtherX[i]
    y[i * AMOUNT_OF_SHAPES] = 0

    # we iterate over the shapes that were read
    for j in range(1, AMOUNT_OF_SHAPES):
        X[i * AMOUNT_OF_SHAPES + j] = readData[j - 1][i]
        y[i * AMOUNT_OF_SHAPES + j] = j

# convert them to numpy arrays
X = np.asarray(X)
y = np.asarray(y)

# we need to hot encode the data (diagonal matrix instead of integers)
y = to_categorical(y)

# get the test data
_, testX = X[:n_train, :], X[n_train:, :]
_, testy = y[:n_train], y[n_train:]

# ===========================================================================
# BUILDING MODEL
# ==========================================================================

# define model
model = Sequential()
model.add(Dense(60, input_dim=60, activation='relu', kernel_initializer='he_uniform'))
# model.add(tf.keras.layers.Dropout(rate = 0.2))
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

# ===========================================================================
# EVALUATING ON OTHER PERSON DRAWINGS
# ==========================================================================

evalueate_user_data(model, "circleJulia.txt", "lightningJulia.txt", "triangleJulia.txt", "starJulia.txt")
evalueate_user_data(model, "circleEti.txt", "lightningEti.txt", "triangleEti.txt", "starEti.txt")
evalueate_user_data(model, "circleLisa.txt", "lightningLisa.txt", "triangleLisa.txt", "starLisa.txt")
evalueate_user_data(model, "circleFatima.txt", "lightningFatima.txt", "triangleFatima.txt", "starFatima.txt")


# save onnx file containing trained network
export_to_onnx(model)
