using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerMovement : MonoBehaviour
{
    private new Rigidbody2D rigidbody;
    private float acceleration = 8.0f;
    private Vector2 movingDirection;
    private PlayerController playerController;

    public Animator anim;
    public SpriteRenderer sprite;

    // Start is called before the first frame update
    void Start()
    {
        rigidbody = GetComponent<Rigidbody2D>();
        playerController = GetComponent<PlayerController>();
    }

    // Update is called once per frame
    void Update()
    {
        float moveHorizontal;
        float moveVertical;

        if (playerController.isPlayer1)
        {
            moveHorizontal = Input.GetAxisRaw("Horizontal");
            moveVertical = Input.GetAxisRaw("Vertical");
        }
        else
        {
            moveHorizontal = Input.GetAxisRaw("Horizontal_2");
            moveVertical = Input.GetAxisRaw("Vertical_2");
        }

        movingDirection = new Vector3(moveHorizontal, moveVertical);

        if (movingDirection.x != 0 || movingDirection.y != 0)
        {
            playerController.SetLookingDirection(new Vector2(Mathf.Round(movingDirection.x), Mathf.Round(movingDirection.y)));
        }

        if(movingDirection.x == 0 && movingDirection.y == 0)
        {
            anim.SetBool("going_sideways", false);
            anim.SetBool("going_up", false);
            anim.SetBool("going_down", false);
            anim.SetBool("is_idle", true);
        }

        if (movingDirection.x != 0)
        {
            anim.SetBool("going_sideways", true);
            anim.SetBool("going_up", false);
            anim.SetBool("going_down", false);
            anim.SetBool("is_idle", false);
            if (movingDirection.x < 0)
            {
                sprite.flipX = true;
            }
            else
            {
                sprite.flipX = false;
            }
        }

        if (movingDirection.y > 0)
        {
            anim.SetBool("going_sideways", false);
            anim.SetBool("going_up", true);
            anim.SetBool("going_down", false);
            anim.SetBool("is_idle", false);
        }

        if (movingDirection.y < 0)
        {
            anim.SetBool("going_sideways", false);
            anim.SetBool("going_up", false);
            anim.SetBool("going_down", true);
            anim.SetBool("is_idle", false);
        }
    }

    private void FixedUpdate()
    {
        if ((movingDirection.x != 0 && Mathf.Abs(rigidbody.position.y - Mathf.Round(rigidbody.position.y)) <= 0.3f) || (movingDirection.y != 0 && Mathf.Abs(rigidbody.position.x - Mathf.Round(rigidbody.position.x)) <= 0.3f))
        {
            SnapToGrid();
        }
        rigidbody.MovePosition(rigidbody.position + movingDirection * acceleration * Time.fixedDeltaTime);
    }

    private void SnapToGrid()
    {
        if (movingDirection.y != 0)
        {
            rigidbody.position = new Vector2(Mathf.Round(rigidbody.position.x), rigidbody.position.y);
        }
        else
        {
            rigidbody.position = new Vector2(rigidbody.position.x, Mathf.Round(rigidbody.position.y));
        }
    }
}
