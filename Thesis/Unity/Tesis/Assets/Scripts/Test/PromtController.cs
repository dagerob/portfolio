using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

[RequireComponent(typeof(LineGenerator))]
public class PromtController : MonoBehaviour
{
    [SerializeField] GameObject prompt;
    [SerializeField] Text counterTxt;
    LineGenerator line;
    int counter = 0;

    private void Awake()
    {
        line = GetComponent<LineGenerator>();
    }

    private void Update()
    {
        if (line.IsWaitingForUser())
        {
            if (Input.GetKeyDown(KeyCode.Return) || OVRInput.GetDown(OVRInput.Button.One, OVRInput.Controller.RTouch) || OVRInput.GetDown(OVRInput.Button.Two, OVRInput.Controller.RTouch))
            {
                line.KeepThisData();
                prompt.SetActive(false);
                counter++;
            }
            if (Input.GetKeyDown(KeyCode.Delete) || OVRInput.GetDown(OVRInput.Button.PrimaryHandTrigger, OVRInput.Controller.RTouch))
            {
                line.EraseThisData();
                prompt.SetActive(false);
            }
        }

        counterTxt.text = counter.ToString();
    }

    public void ShowPrompt()
    {
        prompt.SetActive(true);
    }
}
