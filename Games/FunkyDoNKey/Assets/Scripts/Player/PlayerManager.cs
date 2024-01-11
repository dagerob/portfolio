using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerManager : Manager
{
    GameManager gameManager;
    MoveDirection moveDirection;
    Action action;

    public enum MoveDirection
    {
        UP, DOWN, RIGHT, LEFT, IDLE
    }

    public enum Action
    {
        NONE, POSE, JUMP, DASH, WALKING, DEAD // cann add more
    }

    void Start()
    {
        gameManager = GameManager.GetInstance();
        animObserver.AssignManager(this);
        moveObserver.AssignManager(this);
        audioObserver.AssignManager(this);
        AddObserver(animObserver);
        AddObserver(moveObserver);
        AddObserver(audioObserver);
    }

    public override void OnUserInputChanged(InputReader.InputsState newState)
    {
        // updates the state of the player
        if (newState.keyRightPressed)
        {
            moveDirection = MoveDirection.RIGHT;
        }
        else if (newState.keyLeftPressed)
        {
            moveDirection = MoveDirection.LEFT;
        }
        else
        {
            moveDirection = MoveDirection.IDLE;
        }
        NotifyObservers(this, false, true);
    }

    public override void OnObjectSpawned()
    {
        // change whatever for spawning
    }

    public override void OnObjectDestroyed()
    {
        // change whatever for killing
    }

    public override void OnAction(int newState)
    {
        action = (Action)newState;
        if(action == Action.DEAD)
        {
            Debug.Log("Start Dead Sequence");
            StartCoroutine(OnDeath());
        }

        NotifyObservers(this, true, false);
    }

    public MoveDirection GetMoveDirection()
    {
        return moveDirection;
    }

    public Action GetAction()
    {
        return action;
    }

    public void ResetAction()
    {
        this.action = Action.NONE;
    }

    private IEnumerator OnDeath()
    {
        yield return new WaitForSeconds(0.1f);
        Debug.Log("Restart Level");
        gameManager.StartRestarLevelProcess();
    }
}
