using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public abstract class ObserverV2 : MonoBehaviour
{
    public abstract void Notify(ManagerV2 manager);
}
