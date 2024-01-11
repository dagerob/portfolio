using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class InputReader : MonoBehaviour
{
    InputsState lastFrameState, thisFrameState;
    GameManager gameManager;

    public class InputsState
    {
        public bool keyRightPressed;
        public bool keyLeftPressed;
        public bool keyPosePressed;
        public bool keyJumpPressed;
        public bool keyDashPressed;
        public bool keyPausePressed;

        public InputsState()
        {
            keyRightPressed = false;
            keyLeftPressed = false;
            keyPosePressed = false;
            keyJumpPressed = false;
            keyDashPressed = false;
            keyPausePressed = false;
        }

        public bool Equals(InputsState inputs)
        {
            if (keyLeftPressed != inputs.keyLeftPressed)
            {
                return false;
            }
            if (keyRightPressed != inputs.keyRightPressed)
            {
                return false;
            }
            if (keyPosePressed != inputs.keyPosePressed)
            {
                return false;
            }
            if (keyJumpPressed != inputs.keyJumpPressed)
            {
                return false;
            }
            if (keyDashPressed != inputs.keyDashPressed)
            {
                return false;
            }
            if (keyPausePressed != inputs.keyPausePressed)
            {
                return false;
            }
            return true;
        }

        public void CopyState(InputsState inputs)
        {
            keyLeftPressed = inputs.keyLeftPressed;
            keyRightPressed = inputs.keyRightPressed;
            keyPosePressed = inputs.keyPosePressed;
            keyJumpPressed = inputs.keyJumpPressed;
            keyDashPressed = inputs.keyDashPressed;
            keyPausePressed = inputs.keyDashPressed;
        }
    }

    private void Start()
    {
        gameManager = GameManager.GetInstance();
        thisFrameState = new InputsState();
        lastFrameState = new InputsState();
    }

    void Update()
    {
        if (Input.GetKey(KeyCode.LeftArrow))
        {
            thisFrameState.keyLeftPressed = true;
        }
        else
        {
            thisFrameState.keyLeftPressed = false;
        }
        if (Input.GetKey(KeyCode.RightArrow))
        {
            thisFrameState.keyRightPressed = true;
        }
        else
        {
            thisFrameState.keyRightPressed = false;
        }
        if (Input.GetKeyDown(KeyCode.A))
        {
            thisFrameState.keyPosePressed = true;
        }
        else
        {
            thisFrameState.keyPosePressed = false;
        }

        if (Input.GetKeyDown(KeyCode.S))
        {
            thisFrameState.keyJumpPressed = true;
        }
        else
        {
            thisFrameState.keyJumpPressed = false;
        }

        if (Input.GetKeyDown(KeyCode.D))
        {
            thisFrameState.keyDashPressed = true;
        }
        else
        {
            thisFrameState.keyDashPressed = false;
        }

        if (Input.GetKeyDown(KeyCode.Escape))
        {
            thisFrameState.keyPausePressed = true;
        }
        else
        {
            thisFrameState.keyPausePressed = false;
        }

        if (!thisFrameState.Equals(lastFrameState))
        {
            // There was a change
            gameManager.OnUserInputChanged(thisFrameState);
        }
        lastFrameState.CopyState(thisFrameState);
    }
}
