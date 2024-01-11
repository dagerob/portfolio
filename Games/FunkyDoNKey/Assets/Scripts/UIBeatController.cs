using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class UIBeatController : MonoBehaviour
{
    public Image simpleBeatBGImage;
    public Image simpleBeatFGImage;
    public Image actionBeatBGImage;
    public Image actionBeatFGImage;

    public Image queuedPoseImage;
    public Image queuedJumpImage;
    public Image queuedDashImage;
    public Image queuedRandomImage;

    public bool isActionBeat = false;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void SetAsActionBeat(bool value)
    {
        if (value)
        {
            simpleBeatBGImage.gameObject.SetActive(false);
            simpleBeatFGImage.gameObject.SetActive(false);
            actionBeatBGImage.gameObject.SetActive(true);
            queuedRandomImage.gameObject.SetActive(true);

        }
        else
        {
            simpleBeatBGImage.gameObject.SetActive(true);
            actionBeatBGImage.gameObject.SetActive(false);
            actionBeatFGImage.gameObject.SetActive(false);
            queuedRandomImage.gameObject.SetActive(false);
        }

        isActionBeat = value;
    }

    public void SetQueuedActionImage(PlayerManagerV2.Command action)
    {
        queuedPoseImage.gameObject.SetActive(false);
        queuedJumpImage.gameObject.SetActive(false);
        queuedDashImage.gameObject.SetActive(false);
        queuedRandomImage.gameObject.SetActive(false);

        if (!isActionBeat)
        {
            return;
        }

        if (action == PlayerManagerV2.Command.POSE)
        {
            queuedPoseImage.gameObject.SetActive(true);
        }
        else if (action == PlayerManagerV2.Command.JUMP)
        {
            queuedJumpImage.gameObject.SetActive(true);
        }
        else if (action == PlayerManagerV2.Command.DASH)
        {
            queuedDashImage.gameObject.SetActive(true);
        }
        else
        {
            queuedRandomImage.gameObject.SetActive(true);
        }
    }

    public void AccentBeat(bool value) {
        if (isActionBeat)
        {
            actionBeatFGImage.gameObject.SetActive(value);
        }
        else
        {
            simpleBeatFGImage.gameObject.SetActive(value);
        }
    }

    public void Show(bool value)
    {
        gameObject.SetActive(value);
    }

    public void SetActionColor(Color color)
    {
        queuedPoseImage.color = color;
        queuedJumpImage.color = color;
        queuedDashImage.color = color;
        queuedRandomImage.color = color;
    }

    public void ResetActionColor()
    {
        queuedPoseImage.color = Color.white;
        queuedJumpImage.color = Color.white;
        queuedDashImage.color = Color.white;
        queuedRandomImage.color = new Color(0.75f, 0.75f, 0.75f);
    }
}
