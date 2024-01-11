using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FXObserverV2 : ObserverV2
{
    public GameObject particles;
    public GameObject jumpFX;
    public GameObject landFX;
    public GameObject poseFX;
    private PlayerManagerV2.Action runningAction;

    public override void Notify(ManagerV2 manager)
    {
        PlayerManagerV2 managerPlayer = (PlayerManagerV2)manager;
        runningAction = managerPlayer.GetCurrentAction();
        var shaker = Camera.main.gameObject.GetComponent<CameraController>();
        switch (runningAction)
        {
            case PlayerManagerV2.Action.JUMP_START:
                Instantiate(jumpFX, transform.position, transform.rotation);
                break;
            case PlayerManagerV2.Action.JUMP_LAND:
                Instantiate(landFX, transform.position, transform.rotation);
                break;
            case PlayerManagerV2.Action.POSE:
                Instantiate(poseFX, transform.position, transform.rotation);
                Instantiate(particles, transform.position, transform.rotation);
                Instantiate(particles, transform.position, transform.rotation);
                Instantiate(particles, transform.position, transform.rotation);
                Instantiate(particles, transform.position, transform.rotation);
                shaker.ShakeCamera();
                break;
            case PlayerManagerV2.Action.WIN:
                StartCoroutine(RunWinEffect());
                shaker.ShakeCamera();
                break;
            case PlayerManagerV2.Action.DASH_START:
                StartCoroutine(RunDashEffect());
                break;
        }
    }

    private IEnumerator RunDashEffect()
    {
        while (runningAction == PlayerManagerV2.Action.DASH_FLY || runningAction == PlayerManagerV2.Action.DASH_START)
        {
            Instantiate(poseFX, transform.position, transform.rotation);
            yield return new WaitForFixedUpdate();
        }
    }

    private IEnumerator RunWinEffect()
    {
        while (runningAction == PlayerManagerV2.Action.WIN)
        {
            Instantiate(poseFX, transform.position, transform.rotation);
            Instantiate(particles, transform.position, transform.rotation);
            yield return new WaitForSeconds(0.1f);
        }
    }
}
