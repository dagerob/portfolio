using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AnimObserverPlayer : Observer
{
    PlayerManager.Action runningAction = PlayerManager.Action.NONE;
    PlayerManager manager;
    SpriteRenderer spriteRenderer;
    Animator anim;
    public GameObject particles;
    private bool isDead = false;

    // Start is called before the first frame update
    void Start()
    {
        Debug.Log("Anim Observer Created");
        spriteRenderer = GetComponentInChildren<SpriteRenderer>();
        anim = GetComponentInChildren<Animator>();
        anim.SetBool("isIdle", true);
    }

    public override void Notify(bool wasAction, bool wasDirection)
    {
        PlayerManager.Action thisAction = manager.GetAction();
        if (!isDead && wasAction)
        {
            runningAction = thisAction;
            if (thisAction == PlayerManager.Action.JUMP)
            {
                anim.SetBool("isJumping", true);
                anim.SetBool("isIdle", false);
                anim.SetBool("isWalking", false);
                anim.SetInteger("poseType", 0);
            }else if(thisAction == PlayerManager.Action.DASH)
            {
                anim.SetBool("isJumping", false);
                anim.SetBool("isIdle", false);
                anim.SetBool("isWalking", false);
                anim.SetInteger("poseType", 0);
                StartCoroutine(RunDashEffect());
            }
            else if (thisAction == PlayerManager.Action.POSE)
            {
                anim.SetBool("isJumping", false);
                anim.SetBool("isIdle", false);
                anim.SetBool("isWalking", false);
                int randomInt = Random.Range(1, 4);
                anim.SetInteger("poseType", randomInt);
                Instantiate(particles, transform.position, transform.rotation);
            }
            else if (thisAction == PlayerManager.Action.DEAD)
            {
                anim.SetBool("isJumping", false);
                anim.SetBool("isIdle", false);
                anim.SetBool("isWalking", false);
                anim.SetInteger("poseType", 0);
                anim.SetTrigger("isDead");
                isDead = true;
            }

            else if(thisAction == PlayerManager.Action.WALKING)
            {
                if (manager.GetMoveDirection() != PlayerManager.MoveDirection.IDLE)
                {
                    anim.SetBool("isIdle", false);
                    anim.SetBool("isWalking", true);
                    anim.SetBool("isJumping", false);
                    anim.SetInteger("poseType", 0);
                }
                else
                {
                    anim.SetBool("isIdle", true);
                    anim.SetBool("isWalking", false);
                    anim.SetBool("isJumping", false);
                    anim.SetInteger("poseType", 0);
                }
            }
            else
            {
                anim.SetBool("isIdle", true);
                anim.SetBool("isWalking", false);
                anim.SetBool("isJumping", false);
                anim.SetInteger("poseType", 0);
            }
        }

        // check for the direction of movement
        if (manager.GetMoveDirection() == PlayerManager.MoveDirection.RIGHT)
        {
            spriteRenderer.flipX = false;
        }
        else if (manager.GetMoveDirection() == PlayerManager.MoveDirection.LEFT)
        {
            spriteRenderer.flipX = true;
        }
    }

    public override void AssignManager(Manager manager)
    {
        this.manager = (PlayerManager)manager;
    }

    private IEnumerator RunDashEffect()
    {
        while(runningAction == PlayerManager.Action.DASH)
        {
            Instantiate(particles, transform.position, transform.rotation);
            yield return new WaitForSeconds(0.02f);
        }
    }
}
