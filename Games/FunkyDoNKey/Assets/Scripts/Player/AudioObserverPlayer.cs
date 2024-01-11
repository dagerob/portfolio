using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AudioObserverPlayer : Observer
{
    PlayerManager manager;
    public AudioSource jumpAudio;
    public AudioSource dashAudio;
    public AudioSource poseAudio;
    public AudioSource dieAudio;

    void Start()
    {
        Debug.Log("Audio Observer Created");
    }

    public override void Notify(bool wasAction, bool wasDirection)
    {
        if (wasAction)
        {
            switch (manager.GetAction())
            {
                case PlayerManager.Action.JUMP:
                    jumpAudio.Play();
                    break;
                case PlayerManager.Action.DASH:
                    dashAudio.Play();
                    break;
                case PlayerManager.Action.POSE:
                    poseAudio.Play();
                    var shaker = Camera.main.gameObject.GetComponent<CameraController>();
                    shaker.ShakeCamera();
                    break;
                case PlayerManager.Action.DEAD:
                    dieAudio.Play();
                    break;
            }
        }
    }

    public override void AssignManager(Manager manager)
    {
        this.manager = (PlayerManager)manager;
    }
}
