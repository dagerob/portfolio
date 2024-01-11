﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GoalSpriteChanger : MonoBehaviour
{
    SpriteRenderer spriteRenderer;
    GameManager gameManager;
    public Sprite sprite0, sprite1;

    // Start is called before the first frame update
    void Start()
    {
        spriteRenderer = GetComponentInChildren<SpriteRenderer>();
        gameManager = GameManager.GetInstance();
    }

    // Update is called once per frame
    void Update()
    {
        switch (gameManager.GetTileTypeIndex())
        {
            case 0:
                spriteRenderer.sprite = sprite0;
                break;
            case 1:
                spriteRenderer.sprite = sprite1;
                break;
            case 2:
                spriteRenderer.sprite = sprite0;
                break;
            case 3:
                spriteRenderer.sprite = sprite1;
                break;
        }
    }
}
