using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CharacterSelector : MonoBehaviour
{
    public GameObject arrowUpRed, arrowDownRed, arrowLeftRed, arrowRightRed;
    public GameObject arrowUpBlue, arrowDownBlue, arrowLeftBlue, arrowRightBlue;
    public GameObject badCharacter, rightCharacher;
    // Start is called before the first frame update
    void Start()
    {
        eraseCharacter();
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void setCharacter(int character)
    {
        eraseCharacter();
        switch (character)
        {
            case 0:
                arrowUpBlue.SetActive(true);
                break;
            case 1:
                arrowDownBlue.SetActive(true);
                break;
            case 2:
                arrowLeftBlue.SetActive(true);
                break;
            case 3:
                arrowRightBlue.SetActive(true);
                break;
            case 4:
                arrowUpRed.SetActive(true);
                break;
            case 5:
                arrowDownRed.SetActive(true);
                break;
            case 6:
                arrowLeftRed.SetActive(true);
                break;
            case 7:
                arrowRightRed.SetActive(true);
                break;
            case 8:
                badCharacter.SetActive(true);
                break;
            case 9:
                rightCharacher.SetActive(true);
                break;
        }
    }

    public void eraseCharacter()
    {
        arrowUpRed.SetActive(false);
        arrowDownRed.SetActive(false);
        arrowLeftRed.SetActive(false);
        arrowRightRed.SetActive(false);
        arrowUpBlue.SetActive(false);
        arrowDownBlue.SetActive(false);
        arrowLeftBlue.SetActive(false);
        arrowRightBlue.SetActive(false);
        badCharacter.SetActive(false);
        rightCharacher.SetActive(false);
    }

    public void setOkCharacter()
    {
        eraseCharacter();
        rightCharacher.SetActive(true);
    }

    public void setBadCharacter()
    {
        eraseCharacter();
        badCharacter.SetActive(true);
    }
}
