using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlatformController : MonoBehaviour
{
    public float speed;
    public PlatformMovingDirection initialMovingDirection;

    private new Rigidbody2D rigidbody2D;
    private Vector2 movingDirection;

    public enum PlatformMovingDirection
    {
        UP, DOWN, RIGHT, LEFT
    }

    // Start is called before the first frame update
    void Start()
    {
        rigidbody2D = GetComponent<Rigidbody2D>();

        if(initialMovingDirection == PlatformMovingDirection.RIGHT)
        {
            movingDirection = Vector2.right;
        }
        else if(initialMovingDirection == PlatformMovingDirection.LEFT)
        {
            movingDirection = Vector2.left;
        }
        else if (initialMovingDirection == PlatformMovingDirection.UP)
        {
            movingDirection = Vector2.up;
        }
        else if (initialMovingDirection == PlatformMovingDirection.DOWN)
        {
            movingDirection = Vector2.down;
        }
    }

    private void FixedUpdate()
    {
        int layermask = LayerMask.GetMask("Floor");

        float distance = 0.5f;
        if(movingDirection == Vector2.up || movingDirection == Vector2.down)
        {
            distance = 1.5f;
        }

        RaycastHit2D hit = Physics2D.Raycast(transform.position, movingDirection, distance, layermask);
        if (hit.collider != null)
        {
            if(movingDirection == Vector2.right)
            {
                movingDirection = Vector2.left;
            }
            else if (movingDirection == Vector2.left)
            {
                movingDirection = Vector2.right;
            }
            else if (movingDirection == Vector2.up)
            {
                movingDirection = Vector2.down;
            }
            else if (movingDirection == Vector2.down)
            {
                movingDirection = Vector2.up;
            }

        }
        rigidbody2D.velocity = movingDirection * speed;
    }
}
