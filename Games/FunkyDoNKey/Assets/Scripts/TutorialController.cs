using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TutorialController : MonoBehaviour
{
    public GameObject uIStep0;
    public GameObject uIStep1;
    public GameObject uIStep2;
    public GameObject uIStep3;
    public GameObject uIStep4;

    private GameManager gameManager;

    // Start is called before the first frame update
    void Start()
    {
        gameManager = GameManager.GetInstance();

        gameManager.SetJumpEnabled(false);
        gameManager.SetDashEnabled(false);
        gameManager.SetPoseEnabled(false);
        gameManager.SetRandomCommandEnabled(false);
        gameManager.SetFunkyBarEnabled(false);

        uIStep0.SetActive(true);
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void setTutorialStep(int step)
    {
        uIStep0.SetActive(false);
        uIStep1.SetActive(false);
        uIStep2.SetActive(false);
        uIStep3.SetActive(false);
        uIStep4.SetActive(false);

        if (step == 1)
        {
            gameManager.SetJumpEnabled(true);
            uIStep1.SetActive(true);
        }
        else if (step == 2)
        {
            gameManager.SetDashEnabled(true);
            uIStep2.SetActive(true);
        }
        else if (step == 3)
        {
            gameManager.SetPoseEnabled(true);
            uIStep3.SetActive(true);
        }
        else if (step == 4)
        {
            gameManager.SetFunkyBarEnabled(true);
            uIStep4.SetActive(true);
        }
    }
}
