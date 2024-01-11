using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public abstract class ManagerV2 : MonoBehaviour
{
    protected List<ObserverV2> observers;
    public virtual void AddObserver(ObserverV2 newObserver)
    {
        if(observers ==  null)
        {
            observers = new List<ObserverV2>();
        }
        observers.Add(newObserver);
    }

    public virtual void NotifyObservers()
    {
        foreach(ObserverV2 obs in observers)
        {
            obs.Notify(this);
        }
    }

}
