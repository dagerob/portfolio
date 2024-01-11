import numpy as np

def ReadData(name):
    inputs = []
    with open(name) as f:
        lines = f.readlines()
        for line in lines:
            floatVals = []
            numbers = line.split()
            for num in numbers:
                floatVals.append(float(num))
            floatVals = np.asarray(floatVals)
            inputs.append(floatVals)
    inputs = np.asarray(inputs)
    return inputs