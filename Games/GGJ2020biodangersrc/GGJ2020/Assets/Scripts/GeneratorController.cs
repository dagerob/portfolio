using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GeneratorController : MonoBehaviour
{
    // Start is called before the first frame update
    private GameManager gameManager;
    public Animator animator;

    public bool isRepaired;
    void Start()
    {
        gameManager = GameObject.FindWithTag("GameManager").GetComponent<GameManager>();
    }

    // Update is called once per frame
    void Update()
    {

    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.CompareTag("Player"))
        {
            PlayerController playerController = collision.GetComponent<PlayerController>();
            if (playerController.IsMechanic() && gameManager.HasKey())
            {
                gameManager.ShowTerminal(true);
            }
        }
    }

    public void SetRepaired(bool value)
    {
        animator.SetBool("generator_repaired", value);
        isRepaired = value;
    }
}
