using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class PredictionShower : MonoBehaviour
{
    [System.Serializable]
    public struct Category
    {
        public string name;
        public int cat;
    }

    [SerializeField] List<Category> options;
    [SerializeField] Text result;
    [SerializeField] float acceptedThreshold;

    public void ShowPredictedResult(List<float> results)
    {
        if(results.Count != options.Count)
        {
            Debug.LogError("The NN has more categories than the PredictionShower");
            result.text = "Error";
            return;
        }

        int indexOfLargest = 0;
        int indexOfSecondLargest = 0;
        float largestVal = -10000f;
        float secondLargestVal = -10000f;

        // find the two highest values
        for(int i = 0; i < results.Count; i++)
        {
            if(largestVal < results[i])
            {
                largestVal = results[i];
                indexOfLargest = i;
            }
        }

        for (int i = 0; i < results.Count; i++)
        {
            if(secondLargestVal < results[i] && i != indexOfLargest)
            {
                secondLargestVal = results[i];
                indexOfSecondLargest = i;
            }
        }

        // if the NN is not very sure, discard the result
        if (largestVal >= 0.95f)
        {
            // if the step between both is high, show the one that has the biggest probability to be the right category
            if (largestVal - secondLargestVal >= acceptedThreshold)
            {
                result.text = options[indexOfLargest].name + " " + largestVal;
            }
            else
            {
                result.text = "Undecisive between: " + options[indexOfLargest].name + " " + largestVal+  " and " + options[indexOfSecondLargest].name + " " + secondLargestVal;
            }
        }
        else
        {
            result.text = "Other, largest val was: " + options[indexOfLargest].name + " with " + largestVal;
        }

    }
}
