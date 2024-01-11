using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class MenuController : MonoBehaviour
{
    public Canvas MainSelectionCanvas;
    public Canvas InstructionsCanvas;
    public Canvas CreditsCanvas;
    public string startLevel;
    // Start is called before the first frame update
    void Start()
    {
        MainSelectionCanvas.gameObject.SetActive(true);
    }

    public void StartGame()
    {
        SceneManager.LoadScene(startLevel);
    }

    public void ShowInstructions()
    {
        MainSelectionCanvas.gameObject.SetActive(false);
        InstructionsCanvas.gameObject.SetActive(true);
    }

    public void ShowMainSelection()
    {
        MainSelectionCanvas.gameObject.SetActive(true);
        InstructionsCanvas.gameObject.SetActive(false);
        CreditsCanvas.gameObject.SetActive(false);
    }

    public void ShowCredits()
    {
        MainSelectionCanvas.gameObject.SetActive(false);
        CreditsCanvas.gameObject.SetActive(true);
    }
}
