using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Tilemaps;
using UnityEngine.SceneManagement;

public class LevelController : MonoBehaviour
{
    public PlayerManagerV2 player;
    public UIManager uiManager;
    public BeatController beatController;
    public List<Tilemap> tilemaps;

    public int levelNumber;
    public string nextLevel;
    
    private GameManager gameManager;

    void Start()
    {
        gameManager = GameManager.GetInstance();
        gameManager.SetLevelController(this);
        gameManager.SetPlayer(player);
        gameManager.SetUIManager(uiManager);
        gameManager.SetTileMaps(tilemaps);
        gameManager.SetBeatController(beatController);

        gameManager.SetCurrentLevel(levelNumber);

        gameManager.RevealLevel();
    }

    public void AdvanceLevel()
    {
        SceneManager.LoadScene(nextLevel);
    }

    public void RestartLevel()
    {
        SceneManager.LoadScene(SceneManager.GetActiveScene().name);
    }

    public void GoToMenu()
    {
        SceneManager.LoadScene("Menu");
    }
}
