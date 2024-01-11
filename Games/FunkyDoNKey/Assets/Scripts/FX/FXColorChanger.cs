using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FXColorChanger : MonoBehaviour
{
    private float animationTimer;
    private SpriteRenderer sprite;

    public float animationDuration;
    // Start is called before the first frame update
    void Start()
    {
        animationTimer = 0;
        sprite = GetComponent<SpriteRenderer>();
    }

    // Update is called once per frame
    void Update()
    {
        LerpAnimationColor();
    }

    public void LerpAnimationColor()
    {
        Color animationColor;
        if (animationTimer < animationDuration / 3)
        {
            animationColor = Color.Lerp(Color.red, Color.green, animationTimer / (animationDuration / 3));
        }
        else if (animationTimer < (2 * animationDuration) / 3)
        {
            animationColor = Color.Lerp(Color.green, Color.blue, (animationTimer - (animationDuration / 3)) / (animationDuration / 3));
        }
        else
        {
            animationColor = Color.Lerp(Color.blue, Color.red, (animationTimer - (2 * animationDuration / 3)) / (animationDuration / 3));

            if (animationTimer > animationDuration)
            {
                animationTimer = 0;
            }
        }

        animationTimer += Time.deltaTime;

        sprite.color = animationColor;
    }
}
