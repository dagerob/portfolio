using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using UnityEngine.Events;
[RequireComponent(typeof(LineRenderer))]
public class LineGenerator : MonoBehaviour
{
    LineRenderer line;
    int nextPositionIndex = 0;
    bool savingPoints = false;
    List<GameObject> markers;
    Vector3[] standarizedPositions;
    bool saveThisData = false;
    bool pressedKeyToContinue = false;
    bool waitingForUserDecision = false;

    [SerializeField] Transform cameraTransform;
    [SerializeField] float segmentLenght = 0.1f;
    [SerializeField] GameObject markerObject;
    [SerializeField] int pointsInFinalPath = 19;
    [SerializeField] string savedDataFileName;
    [SerializeField] bool savingData, predictingOutput;
    [SerializeField] NeuralNetworkRunner nn;
    [SerializeField] PredictionShower shower;

    public UnityEvent onFinishedDrawing;

    // Start is called before the first frame update
    void Start()
    {
        line = GetComponent<LineRenderer>();
        markers = new List<GameObject>();
        standarizedPositions = new Vector3[pointsInFinalPath];
    }

    // Update is called once per frame
    void FixedUpdate()
    {
        if (savingPoints)
        {
            float deltaDistance = Vector3.Distance(transform.position, line.GetPosition(nextPositionIndex - 1));
            if(deltaDistance >= segmentLenght)
            {
                AddNewPoint(transform.position);
            }
        }
    }

    private void AddNewPoint(Vector3 point)
    {
        if(nextPositionIndex != 0)
            line.positionCount++;
        line.SetPosition(nextPositionIndex, point);
        nextPositionIndex++;
    }

    private void ResetPoints()
    {
        nextPositionIndex = 0;
        line.positionCount = 1;
        foreach(GameObject obj in markers)
        {
            Destroy(obj);
        }
    }

    public void StartSavingPoints()
    {
        ResetPoints();
        AddNewPoint(transform.position);
        savingPoints = true;
    }

    public void StopSavingPoints()
    {
        savingPoints = false;
        StartCoroutine(SavePathRoutine());
    }

    private IEnumerator SavePathRoutine()
    {
        Vector3[] positions = new Vector3[line.positionCount + 1];
        line.GetPositions(positions);

        // calculate the total distance of the drawn path
        // this will be divided into standarizedPathLength to know how long each segment really has to be
        float totalDistance = 0;
        for(int i = 1; i < line.positionCount; i++)
        {
            totalDistance += Vector3.Distance(positions[i - 1], positions[i]);
        }

        float segmentLength = totalDistance / (pointsInFinalPath - 1);
        // array with the positions that will be sent to the Neuronal Network
        standarizedPositions = new Vector3[pointsInFinalPath];
        // the first position is the same as the old one
        standarizedPositions[0] = positions[0];

        // segmentIndex is the index of the second point of each segment previously recorded
        // we start with 1 because 0 would be the first point of the first segment
        int segmentIndex = 0;
        // restDistance is the distance that remained from last segment
        // we start with 0 because the first point in the final point is the same as the old recorded point -> no remaining distance
        float restDistance = 0.0f;
        // i is the point of the final path we are setting now
        // we start with 1 becuase 0 is the same as in the old path
        for (int i = 1; i < pointsInFinalPath; i++)
        {
            // remainingDistance how much distance we still need to move to set a point
            float remainingDistance = segmentLength;

            while(restDistance < remainingDistance)
            {
                // there is not enough rest distance, use it all up by substratcting it from the remaining distance
                remainingDistance -= restDistance;
                // move to the next segment
                segmentIndex++;
                // if we went over all the points, just set the last old position as the new last position
                if (segmentIndex == positions.Length)
                {
                    // we put the last position farther away so that a point can be set at the right distance
                    Vector3 lastSegmentDisplacement = (positions[segmentIndex - 1] - positions[segmentIndex - 2]).normalized * remainingDistance;
                    positions[segmentIndex] = positions[segmentIndex - 1] + lastSegmentDisplacement;
                }
                // restDistance is now the distance between the next points
                restDistance = Vector3.Distance(positions[segmentIndex], positions[segmentIndex - 1]);
            }

            // this means that we do not need to go to the next segment, there is enough distance in this segment
            restDistance -= remainingDistance; // because we are using this distance, we subtract it from the remaining distance

            // we have to set the point in the corresponding position
            // get a vector from final point of segment to start point of segmen, normalize it and multiply with rest distance.
            Vector3 displacement = (positions[segmentIndex - 1] - positions[segmentIndex]).normalized * restDistance;
            standarizedPositions[i] = positions[segmentIndex] + displacement;
        }

        // transform the world position of these points to the local position based on the current transform of the camera
        for(int i = 0; i < standarizedPositions.Length; i++)
        {
            standarizedPositions[i] = cameraTransform.InverseTransformPoint(standarizedPositions[i]);
        }

        // find the point further right and substract this x value from all points to set everything to start in x = 0
        // do the same on th y and z axes
        float smallestX = 10000;
        float smallestY = 10000;
        float smallestZ = 10000;
        foreach(Vector3 pos in standarizedPositions)
        {
            if(pos.x < smallestX)
            {
                smallestX = pos.x;
            }
            if(pos.y < smallestY)
            {
                smallestY = pos.y;
            }
            if(pos.z < smallestZ)
            {
                smallestZ = pos.z;
            }
        }
        Vector3 correction = new Vector3(smallestX, smallestY, smallestZ);
        for(int i = 0; i < standarizedPositions.Length; i++)
        {
            standarizedPositions[i] -= correction;
        }

        // also find the point furthest away and divide everything throug it to put it in space 0 - 1
        float biggestComponent = -1000;
        foreach(Vector3 pos in standarizedPositions)
        {
            if (Mathf.Abs(pos.x) > biggestComponent)
                biggestComponent = Mathf.Abs(pos.x);
            if (Mathf.Abs(pos.y) > biggestComponent)
                biggestComponent = Mathf.Abs(pos.y);
            if (Mathf.Abs(pos.z) > biggestComponent)
                biggestComponent = Mathf.Abs(pos.z);
        }

        for (int i = 0; i < standarizedPositions.Length; i++)
        {
            standarizedPositions[i] /= biggestComponent;    
        }

        foreach (Vector3 pos in standarizedPositions)
        {
            markers.Add(Instantiate(markerObject, pos, Quaternion.identity));
        }

        // call the onFinishedDrawing Event and wait for the user to make a decision
        onFinishedDrawing.Invoke();
        waitingForUserDecision = true;
        while (!pressedKeyToContinue)
        {
            yield return null;
        }
        waitingForUserDecision = false;
        pressedKeyToContinue = false;

        // depending on the configurations what has to be done now
        if (savingData)
        {
            if (saveThisData)
            {
                WriteData();
            }
        }

        if (predictingOutput)
        {
            float[] separateVals = new float[standarizedPositions.Length * 3];
            for(int i = 0; i < standarizedPositions.Length; i++)
            {
                separateVals[i * 3] = standarizedPositions[i].x;
                separateVals[i * 3 + 1] = standarizedPositions[i].y;
                separateVals[i * 3 + 2] = standarizedPositions[i].z;
            }
            List<float> result = nn.ProcessData(separateVals);
            shower.ShowPredictedResult(result);
        }

        ResetPoints();
        
    }

    public void KeepThisData()
    {
        pressedKeyToContinue = true;
        saveThisData = true;
    }

    public void EraseThisData()
    {
        pressedKeyToContinue = true;
        saveThisData = false;
    }

    public bool IsWaitingForUser()
    {
        return waitingForUserDecision;
    }

    public void WriteData()
    {
        string path = Application.persistentDataPath + "/" + savedDataFileName + ".txt";
        StreamWriter writer = new StreamWriter(path, true);
        foreach(Vector3 pos in standarizedPositions)
        {
            writer.Write(pos.x + " " + pos.y + " " + pos.z + " ");
        }
        writer.Write("\n");
        writer.Close();
    }

    public void ReadString()
    {
        string path = Application.persistentDataPath + "/test.txt";
        //Read the text from directly from the test.txt file
        StreamReader reader = new StreamReader(path);
        Debug.Log(reader.ReadToEnd());
        reader.Close();
    }
}
