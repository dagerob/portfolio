using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class control : MonoBehaviour
{

    // Start is called before the first frame update
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
    }

    public void OpenScene(string newScene)
    {
        Debug.Log("YAY");
        SceneManager.LoadScene(newScene); //si tratan de abrir una scena y no esta , no lo va a abrir
        //SceneManager.LoadScene(newScene).name();
        //playerPreferences.Get....(buscar en internet)
    }

    public void exitOut()
    {
        Application.Quit();
    }
}
