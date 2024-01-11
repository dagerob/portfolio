using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Unity.Barracuda;

public class NeuralNetworkRunner : MonoBehaviour
{
    [SerializeField] NNModel onnxModel;
    [SerializeField] int inputArrayLength = 60;
    Model compiledModel;
    private void Awake()
    {
        compiledModel = ModelLoader.Load(onnxModel);
    }

    public List<float> ProcessData(float[] inputArray)
    {
        if(inputArray.Length != inputArrayLength)
        {
            Debug.LogError("The input Array length does not match the NN input layer size, it musst be: " + inputArrayLength);
            return new List<float>();
        }

        string[] additionalOutputs = new string[] { "StatefulPartitionedCall/sequential/dense/Softmax" };
        IWorker engine = WorkerFactory.CreateWorker(compiledModel,/* additionalOutputs,*/ WorkerFactory.Device.GPU, false);

        Tensor tensorInput = new Tensor(1, inputArrayLength, inputArray);
        engine.Execute(tensorInput);

        Tensor prediction = engine.PeekOutput();

        float[] values = prediction.AsFloats();
        List<float> outputs = new List<float>(values);

        prediction.Dispose();
        engine.Dispose();
        tensorInput.Dispose();

        return outputs;
    }
}
