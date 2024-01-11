using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AudioObserver : ObserverV2
{
    public AudioSource jumpAudio;
    public AudioSource dashAudio;
    public AudioSource poseAudio;
    public AudioSource dieAudio;

    public override void Notify(ManagerV2 manager)
    {
        PlayerManagerV2 managerPlayer = (PlayerManagerV2)manager;
        switch (managerPlayer.GetCurrentAction())
        {
            case PlayerManagerV2.Action.JUMP_START:
                jumpAudio.Play();
                break;
            case PlayerManagerV2.Action.DASH_START:
                dashAudio.Play();
                break;
            case PlayerManagerV2.Action.POSE:
                poseAudio.Play();
                break;
            case PlayerManagerV2.Action.DEAD:
                dieAudio.Play();
                break;
        }
    }
}
