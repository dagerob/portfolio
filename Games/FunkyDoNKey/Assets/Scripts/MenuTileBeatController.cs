using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Tilemaps;

public class MenuTileBeatController : MonoBehaviour
{
    public AudioSource mainAudio;

    public int bars = 4;

    public Tilemap tilemap;

    private int currentBeat;

    public int tileTypeIndex = 1;

    // Start is called before the first frame update
    void Start()
    {
        mainAudio.Play();
        currentBeat = 0;
        ChangeTilemaps();
    }

    private void FixedUpdate()
    {

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
                ChangeTilemaps();
            }
        }
        if (barTime >= (barLength / 4.0d) && barTime <= (barLength / 4.0d) + treshold)
        {
            if (currentBeat != 1)
            {
                currentBeat = 1;
                ChangeTilemaps();
            }
        }
        if (barTime >= (barLength / 2.0d) && barTime <= (barLength / 2.0d) + treshold)
        {
            if (currentBeat != 2)
            {
                currentBeat = 2;
                ChangeTilemaps();
            }
        }
        if (barTime >= ((3 * barLength) / 4.0d) && barTime <= ((3 * barLength) / 4.0d) + treshold)
        {
            if (currentBeat != 3)
            {
                currentBeat = 3;
                ChangeTilemaps();
            }
        }
    }

    private void ChangeTilemaps()
    {
        tilemap.RefreshAllTiles();
        if (tileTypeIndex < 3)
        {
            tileTypeIndex++;
        }
        else
        {
            tileTypeIndex = 0;
        }
    }
}
