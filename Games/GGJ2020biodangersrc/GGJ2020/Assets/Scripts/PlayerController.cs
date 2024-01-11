using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerController : MonoBehaviour
{
    public bool isPlayer1;
    public bool isMechanic;
    private Vector2 lookingDirection = Vector2.right;
    private GameManager gameManager;

    // Start is called before the first frame update
    void Start()
    {
        gameManager = GameObject.FindWithTag("GameManager").GetComponent<GameManager>();
        Spawn();
    }

    // Update is called once per frame
    void Update()
    {

    }

    public bool IsPlayer1()
    {
        return isPlayer1;
    }

    public void SetLookingDirection(Vector2 direction)
    {
        lookingDirection = direction;
    }

    public Vector2 GetLookingDirection()
    {
        return lookingDirection;
    }

    public void DamagePlayer()
    {
        if (isMechanic)
        {
            gameManager.RetryLevel();
        }
        gameObject.SetActive(false);
    }

    public bool IsMechanic()
    {
        return isMechanic;
    }

    public void Reset()
    {
        gameObject.SetActive(true);
        Spawn();
    }


    private void Spawn()
    {
        GameObject[] gameObjectArray;
        if (isPlayer1)
        {
            gameObjectArray = GameObject.FindGameObjectsWithTag("Player1SpawnPoint");
        }
        else
        {
            gameObjectArray = GameObject.FindGameObjectsWithTag("Player2SpawnPoint");
        }


        foreach (GameObject go in gameObjectArray)
        {
            if (go.activeSelf)
            {
                transform.position = go.transform.position;
                break;
            }
        }
    }
}
