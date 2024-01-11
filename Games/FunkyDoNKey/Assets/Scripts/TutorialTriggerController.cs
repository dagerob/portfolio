using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TutorialTriggerController : MonoBehaviour
{
    public int step = 0;
    public TutorialController tutorialController;

    // Start is called before the first frame update
    void Start()
    {
 
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.CompareTag("Player"))
        {
            tutorialController.setTutorialStep(step);
            Destroy(gameObject);
        }
    }
}
