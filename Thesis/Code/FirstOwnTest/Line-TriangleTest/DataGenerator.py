from random import random


import numpy as np
import random as rand
import matplotlib.pyplot as plt

def generateLine(noise, seed = None):
    rand.seed(seed)
    p1 = np.array([rand.random(),rand.random()])
    p2 = np.array([rand.random(),rand.random()])
    p3 = p1 + ((p2 - p1) * rand.random())

    p1 += rand.random() * noise
    p2 += rand.random() * noise
    p3 += rand.random() * noise

    # X = np.array([p1[0],p2[0],p3[0]])
    # Y = np.array([p1[1],p2[1],p3[1]])
    # plt.scatter(X,Y)
    # plt.show()
    return np.array([p1[0], p1[1], p2[0], p2[1], p3[0], p3[1]])

def generateTriangle(seed = None):
    rand.seed(seed)
    p1 = np.array([rand.random(),rand.random()])
    p2 = np.array([rand.random(),rand.random()])
    p3 = np.array([rand.random(),rand.random()])

    # X = np.array([p1[0],p2[0],p3[0]])
    # Y = np.array([p1[1],p2[1],p3[1]])
    # plt.scatter(X,Y)
    # plt.show()
    return np.array([p1[0], p1[1], p2[0], p2[1], p3[0], p3[1]])

def getData(linesAmount, trianglesAmount):
    data = []
    for i in range(linesAmount):
        data.append((0, generateLine(0.001)))
    for i in range(trianglesAmount):
        data.append((1, generateTriangle()))
    
    # mix the order
    for i in range(linesAmount + trianglesAmount):
        temp = data[i]
        randIndex = rand.randint(0, len(data) - 1)
        data[i] = data[randIndex]
        data[randIndex] = temp


    # save the generated data in two separate arrays
    inputs = []
    outputs = []
    for i in range(len(data)):
        inputs.append(data[i][1])
        outputs.append(data[i][0])

    # turn the python arrays into numpy arrays for tensorflow
    inputs = np.asarray(inputs)
    outputs = np.asarray(outputs)
    return inputs, outputs


# inp, out = getData(3,3)
# print(inp)
# print(out)