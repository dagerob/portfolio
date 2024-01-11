using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AnimObserverV2 : ObserverV2
{
    Animator anim;

    private void Start()
    {
        anim = GetComponentInChildren<Animator>();
    }

    public override void Notify(ManagerV2 manager)
    {
        PlayerManagerV2 managerPlayer = (PlayerManagerV2)manager;
        switch (managerPlayer.GetCurrentAction())
        {
            case PlayerManagerV2.Action.JUMP_START:
            case PlayerManagerV2.Action.JUMP_UP:
            case PlayerManagerV2.Action.JUMP_FLOAT:
                anim.SetBool("isJumping", true);
                anim.SetBool("isIdle", false);
                anim.SetBool("isWalking", false);
                anim.SetInteger("poseType", 0);
                break;
            case PlayerManagerV2.Action.JUMP_DOWN:
                anim.SetBool("isFalling", true);
                break;
            case PlayerManagerV2.Action.JUMP_LAND:
                anim.SetBool("landed", true);
                break;
            case PlayerManagerV2.Action.IDLE:
                anim.SetBool("isIdle", true);
                anim.SetBool("isWalking", false);
                anim.SetBool("isJumping", false);
                anim.SetInteger("poseType", 0);
                anim.SetBool("isFalling", false);
                anim.SetBool("landed", false);
                break;
            case PlayerManagerV2.Action.WALKING:
                anim.SetBool("isIdle", false);
                anim.SetBool("isWalking", true);
                anim.SetBool("isJumping", false);
                anim.SetInteger("poseType", 0);
                break;
            case PlayerManagerV2.Action.POSE:
                anim.SetBool("isJumping", false);
                anim.SetBool("isIdle", false);
                anim.SetBool("isWalking", false);
                int randomInt = Random.Range(1, 4);
                anim.SetInteger("poseType", randomInt);
                break;
            case PlayerManagerV2.Action.WIN:
                anim.SetBool("isJumping", false);
                anim.SetBool("isIdle", false);
                anim.SetBool("isWalking", false);
                anim.SetInteger("poseType", 0);
                anim.SetTrigger("hasWon");
                break;
            case PlayerManagerV2.Action.DEAD:
                anim.SetBool("isJumping", false);
                anim.SetBool("isIdle", false);
                anim.SetBool("isWalking", false);
                anim.SetInteger("poseType", 0);
                anim.SetTrigger("isDead");
                break;
        }
    }
}
