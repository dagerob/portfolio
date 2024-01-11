using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerAttack : MonoBehaviour
{
    public GameObject bulletPrefab;
    public bool isReady = true;
    private float reloadCounter = 0.0f;
    private float reloadTime = 1.0f;

    private PlayerController playerController;
    public Animator anim;
    public AudioSource source;


    // Start is called before the first frame update
    void Start()
    {
        playerController = GetComponent<PlayerController>();
    }

    // Update is called once per frame
    void Update()
    {

        if (playerController.IsPlayer1())
        {
            if (Input.GetAxisRaw("Fire1") > 0 && isReady)
            {
                Instantiate(bulletPrefab, transform.position, Quaternion.identity).GetComponent<BulletController>().SetMovingDirection(playerController.GetLookingDirection());
                isReady = false;
                anim.SetTrigger("shoot");
                source.Play();
            }

            if (Input.GetAxisRaw("Reload1") > 0 && !isReady)
            {
                reloadCounter += Time.deltaTime;
                if(reloadCounter >= reloadTime)
                {
                    isReady = true;
                    reloadCounter = 0.0f;
                    Debug.Log("RELOADED");
                }
            }
            else
            {
                reloadCounter = 0.0f;
            }
        }
    }
}
