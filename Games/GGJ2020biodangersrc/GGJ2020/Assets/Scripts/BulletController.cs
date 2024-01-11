using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BulletController : MonoBehaviour
{
    private Vector2 movingDirection = Vector2.zero;
    private float speed = 10.0f;
    private Rigidbody2D rigidbody;
    // Start is called before the first frame update
    void Start()
    {
        rigidbody = GetComponent<Rigidbody2D>();
    }

    // Update is called once per frame
    void Update()
    {
        rigidbody.position = rigidbody.position + (movingDirection * speed * Time.deltaTime);
    }

    public void SetMovingDirection(Vector2 direction)
    {
        movingDirection = direction;
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.CompareTag("Enemy"))
        {
            Debug.Log("HIT");
            collision.GetComponent<EnemyController>().SetFrozen(true);
            Destroy(gameObject);
        }
        else if (collision.CompareTag("Wall"))
        {
            Debug.Log("HIT WALL");
            Destroy(gameObject);
        }
        
    }
}
