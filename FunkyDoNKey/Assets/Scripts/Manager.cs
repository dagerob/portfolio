using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public abstract class Manager : MonoBehaviour
{
    protected List<Observer> observers;

    public Observer animObserver;
    public Observer moveObserver;
    public Observer audioObserver;

    public abstract void OnObjectSpawned();
    public abstract void OnObjectDestroyed();
    public abstract void OnAction(int type);
    public abstract void OnUserInputChanged(InputReader.InputsState newState);

    protected virtual void AddObserver(Observer newObserver)
    {
        if(observers == null)
        {
            observers = new List<Observer>();
        }
        observers.Add(newObserver);
    }

    protected virtual void NotifyObservers(Manager manager, bool wasAction, bool wasDirection)
    {
        foreach (Observer observer in observers)
        {
            observer.Notify(wasAction, wasDirection);
        }
    }
}
