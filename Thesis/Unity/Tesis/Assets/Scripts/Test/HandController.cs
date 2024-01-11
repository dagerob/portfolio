using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class HandController : MonoBehaviour
{
    [SerializeField] OVRInput.Controller controller;
    [SerializeField] GameObject cameraRig;
    bool dragging = false;

    // Update is called once per frame
    void Update()
    {
        // move the hands
        Vector3 pos = OVRInput.GetLocalControllerPosition(controller);
        transform.position = cameraRig.transform.position + cameraRig.transform.TransformDirection(pos);

        Quaternion rot = OVRInput.GetLocalControllerRotation(controller);
        if (controller == OVRInput.Controller.LTouch)
            rot = rot * Quaternion.Euler(0, 0, 90);
        else if (controller == OVRInput.Controller.RTouch)
            rot = rot * Quaternion.Euler(0, 0, -90);
        transform.rotation = cameraRig.transform.rotation * rot;
    }
}
