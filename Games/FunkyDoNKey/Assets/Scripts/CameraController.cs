using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraController : MonoBehaviour {

    public Transform player;

    public float minX;
    public float maxX;
    public float minY;
    public float maxY;

    public float duration;
    public float magnitude;

    private bool isShaking = false;
	
	// Update is called once per frame
	void LateUpdate ()
    {
        if (!isShaking)
        {
            if (transform.position.x > minX)
            {
                if (player.position.x - transform.position.x < -1.0f)
                {
                    transform.position = new Vector3(player.position.x + 1.0f, transform.position.y, transform.position.z);
                }
            }
            if (transform.position.x < maxX)
            {
                if (player.position.x - transform.position.x > 1.0f)
                {
                    transform.position = new Vector3(player.position.x - 1.0f, transform.position.y, transform.position.z);
                }

            }

            if (transform.position.y > minY)
            {
                if (player.position.y - transform.position.y < -1.0f)
                {
                    transform.position = new Vector3(transform.position.x, player.position.y + 1.0f, transform.position.z);
                }
            }
            if (transform.position.y < maxY)
            {
                if (player.position.y - transform.position.y > 1.0f)
                {
                    transform.position = new Vector3(transform.position.x, player.position.y - 1.0f, transform.position.z);
                }

            }
        }
    }

    public void ShakeCamera()
    {
        StartCoroutine(Shake(duration, magnitude));
    }

    private IEnumerator Shake(float duration, float magnitude)
    {
        isShaking = true;
        Vector3 originalPos = transform.position;

        float elapsed = 0.0f;

        while (elapsed < duration)
        {
            float x = Random.Range(-1f, 1f) * magnitude;
            float y = Random.Range(-1f, 1f) * magnitude;

            transform.position += new Vector3(x, y, 0);
            elapsed += Time.deltaTime;
            yield return null;
        }
        transform.position = originalPos;
        isShaking = false;
    }
}
