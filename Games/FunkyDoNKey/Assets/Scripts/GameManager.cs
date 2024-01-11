using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Tilemaps;

public class GameManager : MonoBehaviour
{
    private static GameManager instance;
    private PlayerManagerV2 player;
    private UIManager uiManager;
    private BeatController beatController;
    private LevelController levelController;
    private List<PlayerManagerV2.Command> nextActions = new List<PlayerManagerV2.Command>();
    private bool canAssignAction;
    private List<Tilemap> tilemaps;
    private int tileTypeIndex = 1;

    private int currentLevel = 1;

    private bool comboMode = false;

    private bool levelStarted = false;
    private bool levelCompleted = false;
    private bool playing = false;

    private int funkyness = 0;
    private int funkynessIncreaseFactor = 2;
    private int funkynessDecreaseFactor = 1;

    private bool funkyTime = false;
    private int funkyTimeBarDuration = 4;

    private bool jumpEnabled = true;
    private bool dashEnabled = true;
    private bool poseEnabled = true;
    private bool randomCommandEnabled = true;
    private bool funkyBarEnabled = true;


    private void Awake()
    {
        if (instance != null && instance != this)
        {
            Debug.Log("destroying copy of GameManager");
            Destroy(this.gameObject);
        }
        else if (instance == null)
        {
            Debug.Log("new GameManager");
            instance = this;
        }
        DontDestroyOnLoad(this);
    }

    private void Start()
    {
        StartCoroutine(BeatTest());
    }

    private void Update()
    {
        uiManager.ShowBeat(4, comboMode);
        uiManager.ShowComboModeAnimations(comboMode);
        uiManager.ShowFunkyTimeAnimations(funkyTime);
        uiManager.SetFunkyBarValue(funkyness);
        uiManager.SetLevel(currentLevel);
    }

    public static GameManager GetInstance()
    {
        return instance;
    }

    public void OnUserInputChanged(InputReader.InputsState newState)
    {
        if (newState.keyPausePressed)
        {
            TogglePlaying();
        }

        if (!playing)
        {
            return;
        }

        player.OnUserInputChanged(newState);
        // TODO get the information regarding the action
        // save the last action into the queue if none is saved

        if (!canAssignAction)
        {
            return;
        }

        if ((!comboMode && nextActions.Count == 0) || (comboMode && nextActions.Count <= 1))
        {
            PlayerManagerV2.Command nextAction = PlayerManagerV2.Command.NONE;

            if (newState.keyJumpPressed && jumpEnabled)
            {
                nextAction = PlayerManagerV2.Command.JUMP;
            }
            else if (newState.keyPosePressed && poseEnabled)
            {
                nextAction = PlayerManagerV2.Command.POSE;
            }
            else if (newState.keyDashPressed && dashEnabled)
            {
                nextAction = PlayerManagerV2.Command.DASH;
            }

            if(nextAction != PlayerManagerV2.Command.NONE)
            {
                nextActions.Add(nextAction);
                if (nextActions.Count == 1)
                {
                    uiManager.SetActionImage(3, nextAction);
                }
                else
                {
                    uiManager.SetActionImage(4, nextAction);
                }
            }
        }
    }

    private void OnActionBeat(int beat)
    {
        // TODO call player.OnAction() with the last queued state

        canAssignAction = false;

        if (nextActions.Count == 0)
        {
            if (randomCommandEnabled)
            {
                int randomInt = Random.Range(0, 3);

                if (randomInt == 0)
                {
                    nextActions.Add(PlayerManagerV2.Command.JUMP);
                }
                else if (randomInt == 1)
                {
                    nextActions.Add(PlayerManagerV2.Command.DASH);
                }
                else
                {
                    nextActions.Add(PlayerManagerV2.Command.POSE);
                }
                uiManager.SetActionImage(beat, nextActions[0]);
            }
            else
            {
                nextActions.Add(PlayerManagerV2.Command.NONE);
            }
            
        }

        if (funkyBarEnabled)
        {
            if (nextActions[0] == PlayerManagerV2.Command.POSE)
            {
                IncreaseFunkyness();
            }
            else if (!funkyTime && funkyness < 5 && ((beat == 3 && !comboMode) || beat == 4))
            {
                DecreaseFunkyness();
            }
        }

        if(nextActions[0] != PlayerManagerV2.Command.NONE)
        {
            player.OnNewCommand(nextActions[0]);
        }

        nextActions.RemoveAt(0);
    }


    public void OnBeat(int beat)
    {
        if (beat == 0)
        {
            if (!levelStarted)
            {
                levelStarted = true;
            }

            nextActions.Clear();
            canAssignAction = true;

            uiManager.ClearActionImages();

            if (funkyTime)
            {
                funkyness -= funkynessDecreaseFactor;
                if(funkyness <= 0)
                {
                    funkyTime = false;
                    comboMode = false;
                }
            }

            if (!funkyTime && comboMode && funkyness <= 0)
            {
                comboMode = false;
            }
        }
        else if (beat == 1)
        {
        }
        else if (beat == 2)
        {
        }
        else if(beat == 3)
        {
            OnActionBeat(beat);
        }
        else if(comboMode)
        {
            OnActionBeat(beat);
        }

        uiManager.AccentBeat(beat);
        ChangeTilemaps();
    }

    public void SetFunkyTime(bool value)
    {
        funkyTime = value;
    }

    public bool IsFunkyTime()
    {
        return funkyTime;
    }

    public void SetComboMode(bool value)
    {
        comboMode = value;
    }

    public void SetJumpEnabled(bool value)
    {
        jumpEnabled = value;
    }

    public void SetDashEnabled(bool value)
    {
        dashEnabled = value;
    }

    public void SetPoseEnabled(bool value)
    {
        poseEnabled = value;
    }

    public void SetRandomCommandEnabled(bool value)
    {
        randomCommandEnabled = value;
    }

    public void SetFunkyBarEnabled(bool value)
    {
        funkyBarEnabled = value;
    }

    public bool IsComboMode()
    {
        return comboMode;
    }

    public void SetPlayer(PlayerManagerV2 player)
    {
        this.player = player;
    }

    public void SetUIManager(UIManager uiManager)
    {
        this.uiManager = uiManager;
    }

    public void SetBeatController(BeatController beatController)
    {
        this.beatController = beatController;
    }

    public void SetLevelController(LevelController levelController)
    {
        this.levelController = levelController;
    }

    private IEnumerator BeatTest()
    {
        while (true)
        {
            yield return new WaitForSeconds(2);
            //OnBeat();
        }
    }

    public int GetTileTypeIndex()
    {
        return tileTypeIndex;
    }

    public void SetTileMaps(List<Tilemap> tilemaps)
    {
        this.tilemaps = tilemaps;
    }

    private void ChangeTilemaps()
    {
        foreach(Tilemap tilemap in tilemaps)
        {
            tilemap.RefreshAllTiles();
        }
        if (tileTypeIndex < 3)
        {
            tileTypeIndex++;
        }
        else
        {
            tileTypeIndex = 0;
        }
    }

    private void IncreaseFunkyness()
    {
        if (funkyTime)
        {
            return;
        }

        funkyness += funkynessIncreaseFactor;

        if(funkyness >= 4)
        {
            beatController.ScheduleComboMode();
            if (funkyness >= 5)
            {
                funkyness = 5;
                beatController.ScheduleFunkyTime();
                uiManager.ShowGetReadyText();
            }
        }
    }

    private void DecreaseFunkyness()
    {
        if (funkyTime)
        {
            return;
        }

        funkyness -= funkynessDecreaseFactor;
        
        if (funkyness < 0)
        {
            funkyness = 0;
        }
    }

    public void StartRestarLevelProcess()
    {
        if (!levelStarted)
        {
            return;
        }

        levelStarted = false;
        playing = false;
        uiManager.ShowBlackScreen();
        beatController.SetStoping(true);
    }

    public void StartNextLevelProcess()
    {
        if (!levelStarted)
        {
            return;
        }

        levelStarted = false;
        levelCompleted = true;
        playing = false;
        uiManager.ShowBlackScreen();
    }

    public void RevealLevel()
    {
        funkyness = 2;
        funkyTime = false;
        comboMode = false;
        uiManager.RemoveBlackScreen();
    }

    public void BlackScreenInFront()
    {
        if (levelCompleted)
        {
            levelCompleted = false;
            ChangeLevel();
        }
        else
        {
            RestartLevel();
        }
    }

    public void ResumePlaying()
    {
        playing = true;
    }

    private void ChangeLevel()
    {
        levelController.AdvanceLevel();
    }

    private void RestartLevel()
    {
        levelController.RestartLevel();
    }

    public void TogglePlaying()
    {
        if (!levelStarted)
        {
            return;
        }

        playing = !playing;
        beatController.SetPlaying(playing);
        uiManager.ShowMenuCanvas(!playing);

        if (!playing)
        {
            Time.timeScale = 0;
        }
        else
        {
            Time.timeScale = 1.0f;
        }
    }

    public void QuitGame()
    {
        Application.Quit();
    }

    public void SetCurrentLevel(int level)
    {
        currentLevel = level;
    }
}
