using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class FunkyBarController : MonoBehaviour
{
    private Color barColor = Color.white;

    public Image bar1;
    public Image bar2;
    public Image bar3;
    public Image bar4;
    public Image bar5;

    // Start is called before the first frame update
    void Start()
    {
    }

    // Update is called once per frame
    void Update()
    {
    }

    public void SetValue(int value)
    {

        bar1.color = Color.gray;
        bar2.color = Color.gray;
        bar3.color = Color.gray;
        bar4.color = Color.gray;
        bar5.color = Color.gray;

        if (value >= 1)
        {
            bar1.color = barColor;
        }
        if (value >= 2)
        {
            bar2.color = barColor;
        }
        if (value >= 3)
        {
            bar3.color = barColor;
        }
        if (value >= 4)
        {
            bar4.color = barColor;
        }
        if (value >= 5)
        {
            bar5.color = barColor;
        }
    }

    public void SetColor(Color color)
    {
        barColor = color;
    }
}
