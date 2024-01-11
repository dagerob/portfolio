using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TerminalController : MonoBehaviour
{

    private const int codeLenght = 13;
    public CharacterSelector[] characters = new CharacterSelector[codeLenght];
    private int[] codes = new int[codeLenght];
    private Vector3 positionDown;
    private Vector3 positionUp;
    public float moveDown;
    private int indexToBeEntered = 0;
    private bool isShowing = false;
    private bool needsReset = true;
    private bool isRecordingMovements = false;
    private GameManager gameManager;

    private int prevMoveHorizontal = 0;
    private int prevMoveVertical = 0;
    private int prevMoveHorizontal2 = 0;
    private int prevMoveVertical2 = 0;

    private float terminalMovement = 0.0f;

    private bool generated = false;

    public GameObject holdP1, holdP2, okP1, okP2;
    private bool p1pressed = false, p2pressed = false;

    // Start is called before the first frame update
    void Start()
    {
        positionUp = transform.position;
        positionDown = positionUp - new Vector3(0, moveDown, 0);
        gameManager = GameObject.FindWithTag("GameManager").GetComponent<GameManager>();

        holdP1.SetActive(true);
        holdP2.SetActive(true);
        okP1.SetActive(false);
        okP2.SetActive(false);
    }

    // Update is called once per frame
    void Update()
    {
        if (isShowing)
        {
            if(terminalMovement < 0.99f)
            {
                terminalMovement += Time.deltaTime;
            }
            else
            {
                terminalMovement = 1.0f;
            }
            transform.position = Vector3.Lerp(positionUp, positionDown, terminalMovement);

            

            if (Mathf.RoundToInt(Input.GetAxisRaw("Action1")) == 1)
            {
                p1pressed = true;
                holdP1.SetActive(false);         
                okP1.SetActive(true);
            }

            if (Mathf.RoundToInt(Input.GetAxisRaw("Action2")) == 1)
            {
                p2pressed = true;
                holdP2.SetActive(false);
                okP2.SetActive(true);
            }

            if (p1pressed && p2pressed)
            {
                isRecordingMovements = true;
                holdP1.SetActive(false);
                holdP2.SetActive(false);
                okP1.SetActive(false);
                okP2.SetActive(false);

                if (!generated)
                {
                    for (int i = 0; i < characters.Length; i++)
                    {
                        codes[i] = Random.Range(0, 8);
                        characters[i].setCharacter(codes[i]);
                    }
                    generated = true;
                }
                
            }

            if (isRecordingMovements)
            {
                int moveHorizontal = Mathf.RoundToInt(Input.GetAxisRaw("Horizontal"));
                int moveVertical = Mathf.RoundToInt(Input.GetAxisRaw("Vertical"));
                int moveHorizontal2 = Mathf.RoundToInt(Input.GetAxisRaw("Horizontal_2"));
                int moveVertical2 = Mathf.RoundToInt(Input.GetAxisRaw("Vertical_2"));
                

                if (moveHorizontal != prevMoveHorizontal && moveHorizontal != 0)
                {
                    saveThisMovement(moveHorizontal, 0, 1);
                }
                else if (moveVertical != prevMoveVertical && moveVertical != 0)
                {
                    saveThisMovement(0, moveVertical, 1);
                }
                else if (moveHorizontal2 != prevMoveHorizontal2 && moveHorizontal2 != 0)
                {
                    saveThisMovement(moveHorizontal2, 0, 2);
                }
                else if (moveVertical2 != prevMoveVertical2 && moveVertical2 != 0)
                {
                    saveThisMovement(0, moveVertical2, 2);
                }

                prevMoveHorizontal = moveHorizontal;
                prevMoveVertical = moveVertical;
                prevMoveHorizontal2 = moveHorizontal2;
                prevMoveVertical2 = moveVertical2;
            }
        }
        else
        {
            if (terminalMovement > 0.01f)
            {
                terminalMovement -= Time.deltaTime;
            }
            else
            {
                terminalMovement = 0.0f;

                if (needsReset)
                {
                    resetTerminal();
                    needsReset = false;
                }
            }
            transform.position = Vector3.Lerp(positionUp, positionDown, terminalMovement);
        }
    }

    public void generateSequence()
    {
        isShowing = true;
    }

    public void setCharacter(int index, bool good)
    {
        if (good)
            characters[index].setOkCharacter();
        else
            characters[index].setBadCharacter();
    }

    private void saveThisMovement(int x, int y, int player)
    {
        int enteredCode = 0;
        if(x == 0 && y == 1)
        {
            // up
            enteredCode = 0;
        }else if(x == 0 && y == -1)
        {
            //down
            enteredCode = 1;
        }else if(x == -1 && y == 0)
        {
            //left
            enteredCode = 2;
        }else if(x == 1 && y == 0)
        {
            //right
            enteredCode = 3;
        }
        if(player == 2)
        {
            enteredCode += 4;
        }

        int expectedCode = codes[indexToBeEntered];

        if(enteredCode == expectedCode)
        {
            characters[indexToBeEntered].setOkCharacter();
            moveToNextIndex();
        }
        else
        {
            characters[indexToBeEntered].setBadCharacter();
            badCodeEntered();
        }
    }

    private void moveToNextIndex()
    {
        if (indexToBeEntered < codeLenght-1)
        {
            // other codes still need to be entered
            indexToBeEntered++;
        }
        else
        {
            Debug.Log("GANASTE");
            // the whole code was entered. YOU WON!!!
            gameManager.LevelCompleted();
            resetTerminal();
            // TODO call the win method in game controller
        }
    }
    
    private void resetTerminal()
    {
        for (int i = 0; i < characters.Length; i++)
        {
            characters[i].eraseCharacter();
        }
        codes = new int[codeLenght];
        indexToBeEntered = 0;
        generated = false;
        p1pressed = false;
        p2pressed = false;
        isRecordingMovements = false;
        holdP1.SetActive(true);
        holdP2.SetActive(true);
        okP1.SetActive(false);
        okP2.SetActive(false);
    }

    private void badCodeEntered()
    {
        gameManager.BadCode();
        isRecordingMovements = false;
        isShowing = false;
        needsReset = true;
        prevMoveHorizontal = 0;
        prevMoveVertical = 0;
        prevMoveHorizontal2 = 0;
        prevMoveVertical2 = 0;
        // the code did not match the expected one, notify the gamecontroller
        
        // TODO notify gamecontroller bad code was entered
    }

    public void HideTerminal()
    {
        if (positionUp == Vector3.zero)
        {
            positionUp = transform.position;
            positionDown = positionUp - new Vector3(0, moveDown, 0);
        }
        transform.position = positionUp;
        isRecordingMovements = false;
        isShowing = false;
        needsReset = true;
        prevMoveHorizontal = 0;
        prevMoveVertical = 0;
        prevMoveHorizontal2 = 0;
        prevMoveVertical2 = 0;
        resetTerminal();
    }
}
