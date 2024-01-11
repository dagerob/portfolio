using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BeatController : MonoBehaviour
{
    private GameManager gameManager;

    public AudioSource mainAudio;
    public AudioSource countInAudio;

    public int bars = 4;

    private bool countInDone = false;

    private int currentBeat;

    private bool funkyTimeScheduled;
    private bool comboModeScheduled;

    private bool isPlaying = false;
    private bool isStopping = false;

    private float stoppingTimer = 0.0f;

    // Start is called before the first frame update
    void Start()
    {
        gameManager = GameManager.GetInstance();



#if !UNITY_WEBGL
        countInAudio.PlayScheduled(0.02f);
        mainAudio.PlayScheduled(AudioSettings.dspTime + countInAudio.clip.length + 0.02f);
#else
        countInAudio.Play();
#endif

        countInDone = false;
        isPlaying = true;
    }

    private void FixedUpdate()
    {
        if (!countInDone)
        {
            if(!countInAudio.isPlaying)
            {
                countInDone = true;

#if UNITY_WEBGL
                mainAudio.Play();
#endif

                currentBeat = 0;
                gameManager.ResumePlaying();
                gameManager.OnBeat(currentBeat);
                return;
            }
        }

        if (!isPlaying)
        {
            return;
        }

        if (isStopping)
        {
            stoppingTimer += Time.fixedDeltaTime;

            if(stoppingTimer >= 1)
            {
                stoppingTimer = 1.0f;
            }

            mainAudio.pitch = 1.0f - stoppingTimer;

            return;
        }

        if (gameManager.IsFunkyTime())
        {
            mainAudio.pitch = 1.5f;
        }
        else if (gameManager.IsComboMode())
        {
            mainAudio.pitch = 1.05f;
        }
        else
        {
            mainAudio.pitch = 1.0f;
        }

        float soundLength = mainAudio.clip.length;

        float soundTime = mainAudio.time;

        float barLength = (soundLength / bars);

        float barTime = soundTime;

        while (barTime >= barLength)
        {
            barTime -= barLength;
        }

        float treshold = 0.21f;

        if (barTime <= treshold)
        {
            if (currentBeat != 0)
            {
                currentBeat = 0;
                gameManager.OnBeat(currentBeat);

                if (funkyTimeScheduled)
                {
                    gameManager.SetFunkyTime(true);
                    funkyTimeScheduled = false;
                }

                if (comboModeScheduled)
                {
                    gameManager.SetComboMode(true);
                    comboModeScheduled = false;
                }
            }
        }
        if (barTime >= (barLength / 4.0d) && barTime <= (barLength / 4.0d) + treshold)
        {
            if(currentBeat != 1)
            {
                currentBeat = 1;
                gameManager.OnBeat(currentBeat);
            }
        }
        if (barTime >= (barLength / 2.0d) && barTime <= (barLength / 2.0d) + treshold)
        {
            if (currentBeat != 2)
            {
                currentBeat = 2;
                gameManager.OnBeat(currentBeat);
            }
        }
        if (barTime >= ((3* barLength) / 4.0d) && barTime <= ((3 * barLength) / 4.0d) + treshold)
        {
            if (currentBeat != 3)
            {
                currentBeat = 3;
                gameManager.OnBeat(currentBeat);
            }
        }

        if (gameManager.IsComboMode() && barTime >= ((7 * barLength) / 8.0d) && barTime <= ((7 * barLength) / 8.0d) + treshold)
        {
            if (currentBeat != 4)
            {
                currentBeat = 4;
                gameManager.OnBeat(currentBeat);
            }
        }
    }


    public void ScheduleFunkyTime()
    {
        funkyTimeScheduled = true;
    }

    public void ScheduleComboMode()
    {
        comboModeScheduled = true;
    }

    public void SetStoping(bool value)
    {
        isStopping = value;
    }

    public void SetPlaying(bool value)
    {
        isPlaying = value;
        if (value)
        {
            mainAudio.UnPause();
        }
        else
        {
            mainAudio.Pause();
        }
    }
}
