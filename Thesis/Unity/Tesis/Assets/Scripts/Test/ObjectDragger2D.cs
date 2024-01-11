using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(LineGenerator))]
public class ObjectDragger2D : MonoBehaviour
{
    LineGenerator line;
    [SerializeField] Camera mainCamera;
    bool dragging = false;

    private void Awake()
    {
        line = GetComponent<LineGenerator>();
    }
    void Update()
    {
        if (Input.GetMouseButtonDown(0))
        {
            dragging = true;
        }
        if (Input.GetMouseButtonUp(0))
        {
            line.StopSavingPoints();
            dragging = false;
            Vector3 newPos = Vector3.zero;
            newPos.z = transform.position.z;
            transform.position = newPos;
        }

        if (dragging)
        {
            Vector3 VScreen = new Vector3();
            Vector3 VWorld;

            VScreen.x = Input.mousePosition.x;
            VScreen.y = Input.mousePosition.y;
            VScreen.z = -mainCamera.transform.position.z;
            VWorld = mainCamera.ScreenToWorldPoint(VScreen);

            VWorld.z = transform.position.z;
            transform.position = VWorld;
            if (Input.GetMouseButtonDown(0))
            {
                line.StartSavingPoints();
            }
        }
    }
}
