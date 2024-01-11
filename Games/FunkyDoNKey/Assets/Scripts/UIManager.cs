using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using TMPro;

public class UIManager : MonoBehaviour
{
    public UIBeatController beatController0;
    public UIBeatController beatController1;
    public UIBeatController beatController2;
    public UIBeatController beatController3;
    public UIBeatController beatController4;

    public TextMeshProUGUI textFunkyTime;
    public TextMeshProUGUI textGetReady;
    public TextMeshProUGUI textFunkyBar;
    public TextMeshProUGUI textComboMode;
    public TextMeshProUGUI textFunkyTime2;
    public TextMeshProUGUI textLevel;
    public TextMeshProUGUI textLevelNumber;
    public TextMeshProUGUI textMaxLevel;

    public Image blackScreen;

    public Canvas ingameMenuCanvas;
    public Canvas instructionsCanvas;

    public FunkyBarController funkyBarController;

    private bool showFunkyTimeAnimations = false;
    private bool showComboModeAnimations = false;

    private float animationTimer;
    private float animationDuration = 0.8f;

    private bool showingBlackScreen = false;
    private bool removingBlackScreen = false;
    private bool isBlackScreenActive = true;
    private float blackScreenAnimationTimer = 0.0f;
    private float blackScreenAnimationDuration = 1.0f;

    private Color animationColor = Color.red;

    private Color rotationColor = Color.red;

    private GameManager gameManager;

    // Start is called before the first frame update
    void Start()
    {
        gameManager = GameManager.GetInstance();

        blackScreen.color = new Color(0, 0, 0, 1);
        isBlackScreenActive = true;
        blackScreen.gameObject.SetActive(true);
    }

    // Update is called once per frame
    void Update()
    {
        if (showingBlackScreen)
        {
            blackScreenAnimationTimer += Time.deltaTime;
            blackScreen.color = new Color(0,0,0,(blackScreenAnimationTimer/blackScreenAnimationDuration));
            if (blackScreenAnimationTimer >= blackScreenAnimationDuration)
            {
                isBlackScreenActive = true;
                showingBlackScreen = false;

                gameManager.BlackScreenInFront();
            }
        }
        else if (removingBlackScreen)
        {
            blackScreenAnimationTimer += Time.deltaTime;
            blackScreen.color = new Color(0, 0, 0, 1-(blackScreenAnimationTimer / blackScreenAnimationDuration));
            if (blackScreenAnimationTimer >= blackScreenAnimationDuration)
            {
                isBlackScreenActive = false;
                removingBlackScreen = true;
            }
        }


        if (showFunkyTimeAnimations)
        {
            LerpAnimationColor();
            funkyBarController.SetColor(animationColor);
            textFunkyTime.color = animationColor;
            textFunkyTime.alpha = 0.4f;
            textFunkyTime.gameObject.SetActive(true);
            textGetReady.gameObject.SetActive(false);
            textFunkyBar.color = animationColor;
            textComboMode.color = animationColor;
            textFunkyTime2.color = animationColor;
            textLevel.color = animationColor;
            textLevelNumber.color = animationColor;
            textMaxLevel.color = animationColor;

            beatController0.SetActionColor(animationColor);
            beatController1.SetActionColor(animationColor);
            beatController2.SetActionColor(animationColor);
            beatController3.SetActionColor(animationColor);
            beatController4.SetActionColor(animationColor);
        }
        else
        {
            textFunkyTime.gameObject.SetActive(false);

            beatController0.ResetActionColor();
            beatController1.ResetActionColor();
            beatController2.ResetActionColor();
            beatController3.ResetActionColor();
            beatController4.ResetActionColor();

            textFunkyBar.color = Color.white;
            textLevel.color = Color.white;
            textLevelNumber.color = Color.white;
            textMaxLevel.color = Color.white;
            textFunkyTime2.color = Color.white;

            if (showComboModeAnimations)
            {
                funkyBarController.SetColor(Color.green);
                textComboMode.color = rotationColor;
            }
            else
            {
                textComboMode.color = Color.white;
                funkyBarController.SetColor(Color.white);
            }
        }
    }

    public void SetActionImage(int beat, PlayerManagerV2.Command action)
    {
        if (beat == 0)
        {
            beatController0.SetQueuedActionImage(action);
        }
        else if (beat == 1)
        {
            beatController1.SetQueuedActionImage(action);
        }
        else if (beat == 2)
        {
            beatController2.SetQueuedActionImage(action);
        }
        else if (beat == 3)
        {
            beatController3.SetQueuedActionImage(action);
        }
        else if (beat == 4)
        {
            beatController4.SetQueuedActionImage(action);
        }
    }

    public void ClearActionImages()
    {
        beatController0.SetQueuedActionImage(PlayerManagerV2.Command.NONE);
        beatController1.SetQueuedActionImage(PlayerManagerV2.Command.NONE);
        beatController2.SetQueuedActionImage(PlayerManagerV2.Command.NONE);
        beatController3.SetQueuedActionImage(PlayerManagerV2.Command.NONE);
        beatController4.SetQueuedActionImage(PlayerManagerV2.Command.NONE);
    }

    public void AccentBeat(int beat)
    {
        beatController0.AccentBeat(false);
        beatController1.AccentBeat(false);
        beatController2.AccentBeat(false);
        beatController3.AccentBeat(false);
        beatController4.AccentBeat(false);

        if (beat == 0)
        {
            beatController0.AccentBeat(true);

        }
        else if (beat == 1)
        {
            beatController1.AccentBeat(true);
        }
        else if (beat == 2)
        {
            beatController2.AccentBeat(true);
        }
        else if (beat == 3)
        {
            beatController3.AccentBeat(true);
        }
        else if (beat == 4)
        {
            beatController4.AccentBeat(true);
        }

        RotateAnimationColor();
    }

    public void SetAsActionBeat(int beat, bool value)
    {
        if (beat == 0)
        {
            beatController0.SetAsActionBeat(value);
        }
        else if (beat == 1)
        {
            beatController1.SetAsActionBeat(value);
        }
        else if (beat == 2)
        {
            beatController2.SetAsActionBeat(value);
        }
        else if (beat == 3)
        {
            beatController3.SetAsActionBeat(value);
        }
        else if (beat == 4)
        {
            beatController4.SetAsActionBeat(value);
        }
    }

    public void ShowBeat(int beat, bool value)
    {
        if (beat == 0)
        {
            beatController0.Show(value);
        }
        else if (beat == 1)
        {
            beatController1.Show(value);
        }
        else if (beat == 2)
        {
            beatController2.Show(value);
        }
        else if (beat == 3)
        {
            beatController3.Show(value);
        }
        else if (beat == 4)
        {
            beatController4.Show(value);
        }
    }

    public void SetFunkyBarValue(int value)
    {
        funkyBarController.SetValue(value);
    }

    public void ShowFunkyTimeAnimations(bool value)
    {
        showFunkyTimeAnimations = value;
    }

    public void ShowComboModeAnimations(bool value)
    {
        showComboModeAnimations = value;
    }

    public void ShowGetReadyText()
    {
        textGetReady.gameObject.SetActive(true);
    }

    private void RotateAnimationColor()
    {
        if (rotationColor == Color.red)
        {
            rotationColor = Color.green;
        }
        else if (rotationColor == Color.green)
        {
            rotationColor = Color.magenta;
        }
        else if (rotationColor == Color.magenta)
        {
            rotationColor = Color.cyan;
        }
        else
        {
            rotationColor = Color.red;
        }
    }

    public void LerpAnimationColor()
    {
        if (animationTimer < animationDuration / 3)
        {
            animationColor = Color.Lerp(Color.red, Color.green, animationTimer / (animationDuration / 3));
        }
        else if (animationTimer < (2 * animationDuration) / 3)
        {
            animationColor = Color.Lerp(Color.green, Color.blue, (animationTimer - (animationDuration / 3)) / (animationDuration / 3));
        }
        else
        {
            animationColor = Color.Lerp(Color.blue, Color.red, (animationTimer - (2 * animationDuration / 3)) / (animationDuration / 3));

            if (animationTimer > animationDuration)
            {
                animationTimer = 0;
            }
        }

        animationTimer += Time.deltaTime;
    }

    public void ShowBlackScreen()
    {
        blackScreenAnimationTimer = 0.0f;
        showingBlackScreen = true;
    }

    public void RemoveBlackScreen()
    {
        blackScreenAnimationTimer = 0.0f;
        removingBlackScreen = true;
    }
    public void ShowMenuCanvas(bool value)
    {
        ingameMenuCanvas.gameObject.SetActive(value);
        instructionsCanvas.gameObject.SetActive(false);
    }

    public void QuitGame()
    {
        gameManager.QuitGame();
    }

    public void ShowInstructions()
    {
        ingameMenuCanvas.gameObject.SetActive(false);
        instructionsCanvas.gameObject.SetActive(true);
    }

    public void HideInstructions()
    {
        ingameMenuCanvas.gameObject.SetActive(true);
        instructionsCanvas.gameObject.SetActive(false);
    }

    public void ResumeGame()
    {
        gameManager.TogglePlaying();
    }

    public void SetLevel(int level)
    {
        textLevelNumber.SetText(""+level);
    }
}
