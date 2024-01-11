using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(LineGenerator))]
public class ObjectDragger3D : MonoBehaviour
{
    [SerializeField] OVRInput.Controller controller;
    [SerializeField] Transform hand;
    LineGenerator line;
    Camera mainCamera;
    bool dragging = false;

    private void Awake()
    {
        line = GetComponent<LineGenerator>();
        mainCamera = Camera.main;
    }
    void Update()
    {
        if (OVRInput.GetDown(OVRInput.Button.PrimaryIndexTrigger, controller))
        {
            dragging = true;
        }
        if (OVRInput.GetUp(OVRInput.Button.PrimaryIndexTrigger, controller))
        {
            line.StopSavingPoints();
            dragging = false;
            Vector3 newPos = Vector3.zero;
            newPos.z = transform.position.z;
            transform.position = newPos;
        }

        if (dragging)
        {
            transform.position = hand.position;
            if (OVRInput.GetDown(OVRInput.Button.PrimaryIndexTrigger, controller))
                line.StartSavingPoints();
        }
    }
}
