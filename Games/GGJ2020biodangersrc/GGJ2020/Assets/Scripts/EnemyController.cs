using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EnemyController : MonoBehaviour
{

    private new Rigidbody2D rigidbody;
    private float acceleration = 5.0f;
    private Vector2 movingDirection;

    public LayerMask layerMask;
    public LayerMask layerMask2;

    public int enemyNumber = 1;

    private float pathfindingCooldown = 0.0f;
    private float pathfindingPeriod = 0.2f;

    private bool isFrozen = false;

    private float frozenCooldown = 0.0f;
    private float frozedPeriod = 5.0f;

    public Animator anim;
    public SpriteRenderer sprite;

    // Start is called before the first frame update
    void Start()
    {
        rigidbody = GetComponent<Rigidbody2D>();
        movingDirection = new Vector2(1, 0);

        Spawn();
    }

    // Update is called once per frame
    void Update()
    {
        if (isFrozen)
        {
            frozenCooldown += Time.deltaTime;
            if (frozenCooldown >= frozedPeriod)
            {
                frozenCooldown = 0.0f;
                isFrozen = false;
                anim.SetBool("inabilitated", false);
            }
        }
    }

    private void FixedUpdate()
    {
        if (isFrozen)
        {
            return;
        }

        if(movingDirection.x != 0)
        {
            anim.SetBool("going_sideways", true);
            anim.SetBool("going_up", false);
            anim.SetBool("going_down", false);
            if(movingDirection.x < 0)
            {
                sprite.flipX = true;
            }
            else
            {
                sprite.flipX = false;
            }
        }

        if(movingDirection.y > 0)
        {
            anim.SetBool("going_sideways", false);
            anim.SetBool("going_up", true);
            anim.SetBool("going_down", false);
        }

        if (movingDirection.y < 0)
        {
            anim.SetBool("going_sideways", false);
            anim.SetBool("going_up", false);
            anim.SetBool("going_down", true);
        }

        rigidbody.MovePosition(rigidbody.position + movingDirection * acceleration * Time.fixedDeltaTime);

        pathfindingCooldown += Time.deltaTime;

        if (pathfindingCooldown >= pathfindingPeriod)
        {
            if (SearchPlayer())
            {
                pathfindingCooldown = 0;
            }
            else if (SearchDetour())
            {
                SnapToGrid();
                pathfindingCooldown = 0;
            }
        }

        
    }

    private void OnCollisionEnter2D(Collision2D collision)
    {
        rigidbody.position = new Vector2(Mathf.Round(rigidbody.position.x), Mathf.Round(rigidbody.position.y));


        Vector2 newMovingDirection = Vector2.zero;
        Vector2 raycastDirection1;
        Vector2 raycastDirection2;


        if (movingDirection.x != 0)
        {
            raycastDirection1 = new Vector2(0, 1);
            raycastDirection2 = new Vector2(0, -1);
        }
        else
        {
            raycastDirection1 = new Vector2(1, 0);
            raycastDirection2 = new Vector2(-1, 0);
        }

        if (Physics2D.Raycast(rigidbody.position, raycastDirection1, 1.0f, layerMask))
        {
            if (Physics2D.Raycast(rigidbody.position, raycastDirection2, 1.0f, layerMask))
            {
                newMovingDirection = movingDirection*-1;
            }
            else
            {
                newMovingDirection = raycastDirection2;
            }
        }
        else
        {
            if (Physics2D.Raycast(rigidbody.position, raycastDirection2, 1.0f, layerMask))
            {
                newMovingDirection = raycastDirection1;
            }
            else
            {
                int random = Random.Range(0, 2);

                if (random == 0)
                {
                    newMovingDirection = raycastDirection1;

                }
                else if (random == 1)
                {
                    newMovingDirection = raycastDirection2;
                }
            }
        }
        
        movingDirection = newMovingDirection;

        SnapToGrid();
        pathfindingCooldown = 0;

        if (collision.collider.CompareTag("Player") && !isFrozen)
        {
            collision.collider.GetComponent<PlayerController>().DamagePlayer();
        }
    }


    private void SnapToGrid()
    {
        if (movingDirection.x != 0)
        {
            rigidbody.position = new Vector2(Mathf.Round(rigidbody.position.x), rigidbody.position.y);
        }
        else
        {
            rigidbody.position = new Vector2(rigidbody.position.x, Mathf.Round(rigidbody.position.y));
        }
    }

    private bool SearchDetour()
    {
        Vector2 raycastDirection1 = Vector2.zero;
        Vector2 raycastDirection2 = Vector2.zero;

        if (movingDirection.x != 0 && Mathf.Abs(rigidbody.position.x - Mathf.Round(rigidbody.position.x)) <= 0.10f)
        {
            raycastDirection1 = new Vector2(0, 1);
            raycastDirection2 = new Vector2(0, -1);
        }
        else if (movingDirection.y != 0 && Mathf.Abs(rigidbody.position.y - Mathf.Round(rigidbody.position.y)) <= 0.10f)
        {
            raycastDirection1 = new Vector2(1, 0);
            raycastDirection2 = new Vector2(-1, 0);
        }

        if (raycastDirection1 != Vector2.zero && raycastDirection2 != Vector2.zero)
        {

            List<Vector2> possibleDirections = new List<Vector2>();

            if (!Physics2D.Raycast(rigidbody.position, raycastDirection1, 1.0f, layerMask))
            {
                possibleDirections.Add(raycastDirection1);
            }
            if (!Physics2D.Raycast(rigidbody.position, raycastDirection2, 1.0f, layerMask))
            {
                possibleDirections.Add(raycastDirection2);
            }

            if (possibleDirections.Count > 0)
            {
                int random = Random.Range(0, possibleDirections.Count + 1);

                if (random > 0)
                {
                    movingDirection = possibleDirections[random - 1];
                }

                pathfindingCooldown = 0;
                return true;
            }
        }
        return false;
    }

    private bool SearchPlayer()
    {
        RaycastHit2D raycastHit = Physics2D.Raycast(rigidbody.position, Vector2.up, 10.0f, layerMask2);

        Vector2 newDirection = Vector2.zero;

        if(raycastHit.collider)
        {
            if (raycastHit.collider.CompareTag("Player"))
            {
                newDirection = Vector2.up;
            }
        }

        raycastHit = Physics2D.Raycast(rigidbody.position, Vector2.down, 10.0f, layerMask2);
        if (raycastHit.collider)
        {
            if (raycastHit.collider.CompareTag("Player"))
            {
                newDirection = Vector2.down;
            }
        }

        raycastHit = Physics2D.Raycast(rigidbody.position, Vector2.right, 10.0f, layerMask2);
        if (raycastHit.collider)
        {
            if (raycastHit.collider.CompareTag("Player"))
            {
                newDirection = Vector2.right;
            }
        }

        raycastHit = Physics2D.Raycast(rigidbody.position, Vector2.left, 10.0f, layerMask2);
        if (raycastHit.collider)
        {
            if (raycastHit.collider.CompareTag("Player"))
            {
                newDirection = Vector2.left;
            }
        }

        if(newDirection != Vector2.zero)
        {
            if (Mathf.Abs(newDirection.x) == Mathf.Abs(movingDirection.x) || Mathf.Abs(newDirection.y) == Mathf.Abs(movingDirection.y))
            {
            }
            else
            {
                SnapToGrid();
            }
            movingDirection = newDirection;
            return true;
        }
        return false;
    }

    public void SetFrozen(bool frozen)
    {
        isFrozen = frozen;
        anim.SetBool("inabilitated", true);
    }

    public void Reset()
    {
        gameObject.SetActive(true);
        Spawn();
    }

    private void Spawn()
    {

        string tag = "";
        if(enemyNumber == 1)
        {
            tag = "Enemy1SpawnPoint";
        }else if(enemyNumber == 2)
        {
            tag = "Enemy2SpawnPoint";
        }
        else if(enemyNumber == 3)
        {
            tag = "Enemy3SpawnPoint";
        }
        else{
            tag = "Enemy4SpawnPoint";
        }


        GameObject[] gameObjectArray = GameObject.FindGameObjectsWithTag(tag);

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
