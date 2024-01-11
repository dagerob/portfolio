using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerManagerV2 : ManagerV2
{
    private Action currentAction = Action.IDLE;
    private MoveDirection moveDirection = MoveDirection.NONE;
    private MoveDirection previousDirection = MoveDirection.NONE;
    private Rigidbody2D rigidBody;
    private Collider2D collider;
    private SpriteRenderer sprite;
    private bool stageCleared = false;
    private Transform goalTransform;
    private GameManager gameManager;

    public ObserverV2 animObserver;
    public ObserverV2 fxObserver;
    public ObserverV2 audioObserver;

    public float movementSpeed;
    public int jumpCycles;
    public int jumpDampeningCycles;
    public float jumpInitialVelocity;
    public float dashVelocity;
    public int dashCycles;
    public int dashFloatCycles;
    public float poseLength;
    public float terminalVelocity;
    

    public enum Action
    {
        IDLE,
        POSE,
        JUMP_START,
        JUMP_UP,
        JUMP_FLOAT,
        JUMP_DOWN,
        JUMP_LAND,
        DASH_START,
        DASH_FLY,
        DASH_BREAK,
        WALKING,
        DEAD,
        WIN // cann add more
    }

    public enum MoveDirection
    {
        UP,
        DOWN,
        RIGHT,
        LEFT,
        NONE
    }

    public enum Command
    {
        JUMP, POSE, DASH, NONE
    }

    private void Awake()
    {
        AddObserver(animObserver);
        AddObserver(fxObserver);
        AddObserver(audioObserver);
    }

    private void Start()
    {
        rigidBody = GetComponent<Rigidbody2D>();
        collider = GetComponent<Collider2D>();
        sprite = GetComponentInChildren<SpriteRenderer>();
        gameManager = GameManager.GetInstance();
    }

    public void OnNewCommand(Command command)
    {
        if (!stageCleared)
        {
            switch (command)
            {
                case Command.DASH:
                    StartCoroutine(MakeDash());
                    break;
                case Command.JUMP:
                    StartCoroutine(MakeJump());
                    break;
                case Command.POSE:
                    StartCoroutine(MakePose());
                    break;
            }
        }
    }

    public Action GetCurrentAction()
    {
        return currentAction;
    }

    public void OnUserInputChanged(InputReader.InputsState newState)
    {
        // updates the state of the player
        if (newState.keyRightPressed)
        {
            moveDirection = MoveDirection.RIGHT;
            previousDirection = moveDirection;
            if(currentAction == Action.IDLE)
            {
                ChangeCurrentAction(Action.WALKING);
            }
        }
        else if (newState.keyLeftPressed)
        {
            moveDirection = MoveDirection.LEFT;
            previousDirection = moveDirection;
            if (currentAction == Action.IDLE)
            {
                ChangeCurrentAction(Action.WALKING);
            }
        }
        else
        {
            moveDirection = MoveDirection.NONE;
            if (currentAction == Action.WALKING)
            {
                ChangeCurrentAction(Action.IDLE);
            }
        }
    }

    private void FixedUpdate()
    {
        
        // Player movement
        if (currentAction != Action.DASH_START &&
            currentAction != Action.DASH_FLY &&
            currentAction != Action.WIN &&
            currentAction != Action.DEAD &&
            currentAction != Action.POSE)
        {

            rigidBody.velocity = new Vector2(0, rigidBody.velocity.y);
            CheckForPlatforms();

            if (moveDirection == MoveDirection.RIGHT)
            {
                rigidBody.velocity += new Vector2(movementSpeed, 0);
                sprite.flipX = false;
            }
            else if (moveDirection == MoveDirection.LEFT)
            {
                rigidBody.velocity += new Vector2(-movementSpeed, 0);
                sprite.flipX = true;
            }
        }

        if(currentAction == Action.DEAD)
        {
            rigidBody.velocity = new Vector2(0, rigidBody.velocity.y);
            moveDirection = MoveDirection.NONE;
            StartCoroutine(OnDeath());
        }

        CheckTerminalVelocity();
    }


    private IEnumerator OnDeath()
    {
        yield return new WaitForSeconds(0.1f);
        gameManager.StartRestarLevelProcess();
    }

    private IEnumerator MakeJump()
    {
        ChangeCurrentAction(Action.JUMP_START);
        ChangeCurrentAction(Action.JUMP_FLOAT);
        for (int i = 0; i < jumpCycles; i++)
        {
            int layermask = LayerMask.GetMask("Floor", "Platforms");
            RaycastHit2D hit = Physics2D.Raycast(new Vector2(collider.bounds.center.x + 0.35f, collider.bounds.center.y), Vector2.up, collider.bounds.extents.y + 0.2f, layermask);
            RaycastHit2D hit2 = Physics2D.Raycast(new Vector2(collider.bounds.center.x - 0.35f, collider.bounds.center.y), Vector2.up, collider.bounds.extents.y + 0.2f, layermask);
            if (hit.collider != null || hit2.collider != null)
            {
                break;
            }

            float jumpVelocity = jumpInitialVelocity;

            if (i > jumpCycles - jumpDampeningCycles)
            {
                jumpVelocity = (jumpInitialVelocity) * (float)(jumpCycles - i) / (float)jumpDampeningCycles;
            }

            rigidBody.velocity = new Vector2(rigidBody.velocity.x, jumpVelocity);

            yield return new WaitForFixedUpdate();
        }

        if(currentAction == Action.JUMP_FLOAT)
            ChangeCurrentAction(Action.JUMP_DOWN);

        rigidBody.velocity = new Vector2(rigidBody.velocity.x, -3.0f); ;
        StartCoroutine(CheckIfJumping());
    }

    private IEnumerator CheckIfJumping()
    {
        yield return new WaitForSeconds(0.1f);
        while (!IsGrounded())
        {
            yield return new WaitForFixedUpdate();
        }
        ChangeCurrentAction(Action.JUMP_LAND);
        yield return new WaitForSeconds(0.1f);
        if(moveDirection == MoveDirection.NONE)
            ChangeCurrentAction(Action.IDLE);
        else
            ChangeCurrentAction(Action.WALKING);
    }

    private bool IsGrounded()
    {
        int layermask = LayerMask.GetMask("Floor", "Platforms");
        RaycastHit2D hit = Physics2D.Raycast(new Vector2(collider.bounds.center.x + 0.4f, collider.bounds.center.y), Vector2.down, collider.bounds.extents.y + 0.2f, layermask);
        RaycastHit2D hit2 = Physics2D.Raycast(new Vector2(collider.bounds.center.x - 0.4f, collider.bounds.center.y), Vector2.down, collider.bounds.extents.y + 0.2f, layermask);
        if (hit.collider != null || hit2.collider != null)
        {
            return true;
        }
        return false;
    }

    private void CheckForPlatforms()
    {
        int layermask = LayerMask.GetMask("Platforms");
        RaycastHit2D hit = Physics2D.Raycast(new Vector2(collider.bounds.center.x + 0.4f, collider.bounds.center.y), Vector2.down, collider.bounds.extents.y + 0.2f, layermask);
        RaycastHit2D hit2 = Physics2D.Raycast(new Vector2(collider.bounds.center.x - 0.4f, collider.bounds.center.y), Vector2.down, collider.bounds.extents.y + 0.2f, layermask);
        if (hit.collider != null || hit2.collider != null)
        {
            Rigidbody2D platformRB;
            if (hit.collider != null)
            {
                platformRB = hit.collider.gameObject.GetComponent<Rigidbody2D>();
            }
            else
            {
                platformRB = hit2.collider.gameObject.GetComponent<Rigidbody2D>();
            }

            rigidBody.velocity = new Vector2(platformRB.velocity.x, rigidBody.velocity.y);  
        }
    }

    private void CheckTerminalVelocity()
    {
        if (rigidBody.velocity.y < -terminalVelocity)
        {
            rigidBody.velocity = new Vector2(rigidBody.velocity.x, -terminalVelocity);
        }
    }

    private IEnumerator MakeDash()
    {
        ChangeCurrentAction(Action.DASH_START);
        ChangeCurrentAction(Action.DASH_FLY);
        Vector2 direction;
        if (previousDirection == MoveDirection.LEFT)
        {
            direction = new Vector2(-1, 0);
        }
        else
        {
            direction = new Vector2(1, 0);
        }

        rigidBody.position = new Vector2(rigidBody.position.x, rigidBody.position.y + 0.1f);

        for (int i = 0; i < dashCycles; i++)
        {
            rigidBody.velocity = direction * dashVelocity;
            yield return new WaitForFixedUpdate();
        }

        rigidBody.velocity = Vector2.zero;

        for (int i = 0; i < dashFloatCycles; i++)
        {
            rigidBody.velocity = new Vector2(rigidBody.velocity.x, 0);
            yield return new WaitForFixedUpdate();
        }

        if (currentAction == Action.DASH_FLY)
            if (moveDirection == MoveDirection.NONE)
                ChangeCurrentAction(Action.IDLE);
            else
                ChangeCurrentAction(Action.WALKING);
    }

    private void ChangeCurrentAction(Action newAction)
    {
        if (currentAction != Action.DEAD && !stageCleared)
        {
            Debug.Log(newAction);
            currentAction = newAction;
            NotifyObservers();
        }
    }

    private IEnumerator MakePose()
    {
        ChangeCurrentAction(Action.POSE);
        yield return new WaitForSeconds(poseLength);
        if(currentAction == Action.POSE)
            ChangeCurrentAction(Action.IDLE);
    }

    private void OnTriggerEnter2D(Collider2D collider)
    {
        CheckCommonTriggers(collider);
    }

    private void OnTriggerStay2D(Collider2D collider)
    {
        CheckCommonTriggers(collider);
    }


    private void CheckCommonTriggers(Collider2D collider)
    {
        if (collider.gameObject.tag == "Finish")
        {
            if (!stageCleared)
            {
                stageCleared = true;
                goalTransform = collider.gameObject.transform;
                StartCoroutine(FinishSequence());
            }
        }
        else if (collider.gameObject.tag == "Deadly")
        {
            if (currentAction != Action.DEAD && currentAction != Action.POSE)
            {
                ChangeCurrentAction(Action.DEAD);
            }
        }
        else if (collider.gameObject.tag == "Pit")
        {
            if (currentAction != Action.DEAD)
            {
                ChangeCurrentAction(Action.DEAD);
            }
        }
    }

    private IEnumerator FinishSequence()
    {
        ChangeCurrentAction(Action.WIN);
        moveDirection = MoveDirection.NONE;
        rigidBody.position = new Vector2(rigidBody.position.x, goalTransform.position.y + 0.5f);
        float walkingTimer = 0.0f;

        while (walkingTimer < 1.0f)
        {
            rigidBody.position = new Vector2(Mathf.Lerp(rigidBody.position.x, goalTransform.position.x, walkingTimer), rigidBody.position.y);
            walkingTimer += Time.fixedDeltaTime;
            yield return new WaitForFixedUpdate();
        }
        
        gameManager.StartNextLevelProcess();
    }
}
