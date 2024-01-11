using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class WinDonkeyController : MonoBehaviour
{
    public float speed;
    public float speedText;
    private Animator anim;
    public ParticleSystem particles;
    public RectTransform textTransform;
    public GameObject quitButton;

    private void Start()
    {
        anim = GetComponent<Animator>();
        StartCoroutine(RunWinSequence());
        StartCoroutine(MoveTextUp());
        particles.Stop();
    }

    IEnumerator RunWinSequence()
    {
        while(transform.position.x < 0)
        {
            transform.position += new Vector3(1, 0, 0) * speed * Time.deltaTime;
            yield return null;
        }
        anim.SetTrigger("StartPosing");
        particles.Play();
        quitButton.SetActive(true);
    }

    IEnumerator MoveTextUp()
    {
        while(textTransform.offsetMax.y > 0)
        {
            textTransform.position -= new Vector3(0, 1,0) * speedText * Time.deltaTime;
            yield return null;
        }
        
    }
}
