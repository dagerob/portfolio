using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MoveObserverPlayer : Observer
{
    PlayerManager manager;
    PlayerManager.Action runningAction = PlayerManager.Action.NONE;
    PlayerManager.MoveDirection moveDirection = PlayerManager.MoveDirection.IDLE;
    PlayerManager.MoveDirection previousmoveDirection = PlayerManager.MoveDirection.RIGHT;
    public float velocity;
    private Rigidbody2D rigidBody;
    private BoxCollider2D collider;
    private bool isJumping = false;
    private bool isDashing = false;
    private bool isPosing = false;
    private bool isDead = false;
    public int dashLength;
    public int dashVelMultiplier;
    public float poseLenght;
    public int jumpHeight;
    public int jumpVelMultiplier;

    // Start is called before the first frame update
    void Start()
    {
        Debug.Log("Move Observer Created");

        rigidBody = GetComponent<Rigidbody2D>();
        collider = GetComponent<BoxCollider2D>();
    }

    private void Update()
    {
        if (!isDashing && !isPosing && !isDead)
        {
            if (moveDirection == PlayerManager.MoveDirection.LEFT)
            {
                rigidBody.velocity = new Vector2(-velocity, rigidBody.velocity.y);
                previousmoveDirection = moveDirection;
                if (!isJumping && runningAction != PlayerManager.Action.WALKING)
                {
                    manager.OnAction((int)PlayerManager.Action.WALKING);
                }
            }
            else if (moveDirection == PlayerManager.MoveDirection.RIGHT)
            {
                rigidBody.velocity = new Vector2(velocity, rigidBody.velocity.y);
                previousmoveDirection = moveDirection;
                if (!isJumping && runningAction != PlayerManager.Action.WALKING)
                {
                    manager.OnAction((int)PlayerManager.Action.WALKING);
                }
            }
            else
            {
                int layermask = LayerMask.GetMask("Platforms");
                RaycastHit2D hit = Physics2D.Raycast(transform.position, -transform.up, collider.bounds.extents.y + 0.2f, layermask);
                if (hit.collider == null)
                {
                    rigidBody.velocity = new Vector2(0, rigidBody.velocity.y);
                }
                else
                {
                    Rigidbody2D platformRB = hit.collider.gameObject.GetComponent<Rigidbody2D>();
                    rigidBody.velocity = new Vector2(platformRB.velocity.x, rigidBody.velocity.y);
                }

                if (!isJumping && runningAction != PlayerManager.Action.NONE)
                {
                    manager.OnAction((int)PlayerManager.Action.NONE);
                }
            }

            if (isDead)
            {
                rigidBody.velocity = new Vector2(0, rigidBody.velocity.y);
            }
        }
    }

    public override void Notify(bool wasAction, bool wasDirection)
    {
        if (wasAction)
        {
            PlayerManager.Action thisAction = manager.GetAction();
            RunAction(thisAction);
            runningAction = thisAction;
        }else if (wasDirection)
        {
            moveDirection = manager.GetMoveDirection();
        }
    }

    public override void AssignManager(Manager manager)
    {
        this.manager = (PlayerManager)manager;
    }

    private void RunAction(PlayerManager.Action newAction)
    {
        if (!isDead)
        {
            if (newAction == PlayerManager.Action.JUMP)
            {
                isJumping = true;
                StartCoroutine(MakeJump());
            }
            else if (newAction == PlayerManager.Action.DASH)
            {
                isDashing = true;
                StartCoroutine(MakeDash());
            }
            else if (newAction == PlayerManager.Action.POSE)
            {
                if (isPosing)
                {

                }
                isPosing = true;
                StartCoroutine(MakePose());
            }
            else if (newAction == PlayerManager.Action.DEAD)
            {
                isDead = true;
            }
        }
    }

    private bool IsGrounded()
    {
        int layermask = LayerMask.GetMask("Floor", "Platforms");
        RaycastHit2D hit = Physics2D.Raycast(new Vector2(transform.position.x - 0.4f, transform.position.y), -transform.up, collider.bounds.extents.y + 0.2f, layermask);
        RaycastHit2D hit2 = Physics2D.Raycast(new Vector2(transform.position.x + 0.4f, transform.position.y), -transform.up, collider.bounds.extents.y + 0.2f, layermask);
        if (hit.collider != null || hit2.collider != null)
        {
            return true;
        }
        return false;
    }

    private IEnumerator CheckIfJumping()
    {
        yield return new WaitForSeconds(0.1f);
        while (!IsGrounded())
        {
            yield return new WaitForFixedUpdate();
        }
        isJumping = false;
        manager.OnAction((int)PlayerManager.Action.NONE);
    }

    private IEnumerator MakeDash()
    {
        float dashVelocity = velocity * dashVelMultiplier;
        Vector2 direction;
        if(previousmoveDirection == PlayerManager.MoveDirection.LEFT)
        {
            direction = new Vector2(-1, 0);
        }
        else
        {
            direction = new Vector2(1, 0);
        }

        rigidBody.position = new Vector2(rigidBody.position.x, rigidBody.position.y + 0.2f);

        for (int i = 0; i < dashLength; i++)
        {
            rigidBody.velocity = direction * dashVelocity;
            yield return new WaitForFixedUpdate();
        }
        rigidBody.velocity = Vector2.zero;
        isDashing = false;
        manager.OnAction((int)PlayerManager.Action.NONE);
    }

    private IEnumerator MakeJump()
    {
        for (int i = 0; i < jumpHeight; i++)
        {
            int layermask = LayerMask.GetMask("Floor", "Platforms");
            RaycastHit2D hit = Physics2D.Raycast(transform.position, Vector2.up, collider.bounds.extents.y + 0.2f, layermask);
            if (hit.collider != null)
            {
                break;
            }

            float jumpVelocity = jumpVelMultiplier;

            if(jumpHeight-i < 5)
            {
                jumpVelocity *= (jumpHeight - i)/5;
            }

            rigidBody.velocity = new Vector2(rigidBody.velocity.x, jumpVelocity);
            yield return new WaitForFixedUpdate();
        }
        rigidBody.velocity = new Vector2(rigidBody.velocity.x, -3.0f); ;
        StartCoroutine(CheckIfJumping());
    }

    private IEnumerator MakePose()
    {
        yield return new WaitForSeconds(poseLenght);
        isPosing = false;
        manager.OnAction((int)PlayerManager.Action.NONE);
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if((collision.gameObject.tag == "Deadly") || collision.gameObject.tag == "Pit")
        {
            manager.OnAction((int)PlayerManager.Action.DEAD);
        }else if(collision.gameObject.tag == "Finish")
        {
            GameManager.GetInstance().StartNextLevelProcess();
        }
    }
}
