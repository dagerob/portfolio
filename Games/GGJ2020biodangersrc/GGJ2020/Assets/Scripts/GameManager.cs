using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GameManager : MonoBehaviour
{
    public TerminalController terminal;
    public GeneratorController generator;
    public KeyController keyController;

    public GameObject blackScreen;
    public GameObject blackScreen2;

    public PlayerController player1;
    public PlayerController player2;

    public EnemyController enemy1;
    public EnemyController enemy2;
    public EnemyController enemy3;
    public EnemyController enemy4;

    public GameObject sectorA1;
    public GameObject sectorB1;
    public GameObject sectorC1;
    public GameObject sectorD1;
    public GameObject sectorA2;
    public GameObject sectorB2;
    public GameObject sectorC2;
    public GameObject sectorD2;

    private bool hidingLevel = false;
    private bool revealingLevel = false;
    private bool loadingLevel = false;
    private float animationCounter = 0.0f;

    // Start is called before the first frame update
    private int level = 1;
    private bool hasKey = false;

    private bool showTerminal = false;

    private bool resetLevel = false;

    private bool won = false;

    void Start()
    {
        ChangeSectors();
        ResetLevel();
    }

    // Update is called once per frame
    void Update()
    {
        if (hidingLevel)
        {
            animationCounter += Time.deltaTime;
            if (won)
            {
                blackScreen2.transform.localPosition = new Vector2(0, Mathf.Lerp(1000, 0, animationCounter));
            }
            else
            {
                blackScreen.transform.localPosition = new Vector2(0, Mathf.Lerp(1000, 0, animationCounter));
            }
            
            if(animationCounter >= 1.0f)
            {
                hidingLevel = false;
                ResetLevel();
                loadingLevel = true;
                animationCounter = 0.0f;
            }
        }else if (loadingLevel)
        {
            animationCounter += Time.deltaTime;
            if (animationCounter >= 1.0f)
            {
                loadingLevel = false;
                revealingLevel = true;
                animationCounter = 0.0f;
            }
        }
        else if (revealingLevel)
        {
            animationCounter += Time.deltaTime;
            if (won)
            {
                blackScreen2.transform.localPosition = new Vector2(0, Mathf.Lerp(0, -1000, animationCounter));
            }
            else
            {
                blackScreen.transform.localPosition = new Vector2(0, Mathf.Lerp(0, -1000, animationCounter));
            }
            
            if (animationCounter >= 1.0f)
            {
                revealingLevel = false;
                won = false;
                animationCounter = 0.0f;
            }
        }
    }

    public void SetHasKey(bool value)
    {
        hasKey = value;
    }

    public bool HasKey()
    {
        return hasKey;   
    }

    public void ShowTerminal(bool value)
    {
        showTerminal = value;
        terminal.generateSequence();
        hasKey = false;
    }

    public void LevelCompleted()
    {
        generator.SetRepaired(true);
        terminal.HideTerminal();
        won = true;
        hidingLevel = true;
        hasKey = false;
    }

    public void BadCode()
    {
        showTerminal = false;
        keyController.gameObject.SetActive(true);
    }

    public void RetryLevel()
    {
        hasKey = false;
        hidingLevel = true;
        won = false;
    }

    private void ResetLevel()
    {
        if (won)
        {
            ChangeSectors();
        }

        Spawn();

        player1.Reset();
        player2.Reset();
        enemy1.Reset();
        enemy2.Reset();
        enemy3.Reset();
        enemy4.Reset();
        keyController.gameObject.SetActive(true);
        terminal.HideTerminal();
    }


    private void Spawn()
    {
        GameObject[] generatorObjectArray = GameObject.FindGameObjectsWithTag("GeneratorSpawnPoint"); ;

        foreach (GameObject go in generatorObjectArray)
        {
            if (go.activeSelf)
            {
                generator.gameObject.transform.position = go.transform.position;
                generator.SetRepaired(false);
                break;
            }
        }

        GameObject[] keyObjectArray = GameObject.FindGameObjectsWithTag("KeySpawnPoint"); ;

        foreach (GameObject go in keyObjectArray)
        {
            if (go.activeSelf)
            {
                keyController.gameObject.transform.position = go.transform.position;
                keyController.gameObject.SetActive(true);
                break;
            }
        }
    }


    private void ChangeSectors()
    {
        int random = Random.Range(0, 4);

        sectorA1.gameObject.SetActive(false);
        sectorB1.gameObject.SetActive(false);
        sectorC1.gameObject.SetActive(false);
        sectorD1.gameObject.SetActive(false);
        sectorA2.gameObject.SetActive(true);
        sectorB2.gameObject.SetActive(true);
        sectorC2.gameObject.SetActive(true);
        sectorD2.gameObject.SetActive(true);

        if (random == 0)
        {
            sectorA2.gameObject.SetActive(false);
            sectorA1.gameObject.SetActive(true);
        }
        else if (random == 1)
        {
            sectorB2.gameObject.SetActive(false);
            sectorB1.gameObject.SetActive(true);
        }
        else if (random == 2)
        {
            sectorC2.gameObject.SetActive(false);
            sectorC1.gameObject.SetActive(true);
        }
        else
        {
            sectorD2.gameObject.SetActive(false);
            sectorD1.gameObject.SetActive(true);
        }
    }
}
