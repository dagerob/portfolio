using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Tilemaps;

[CreateAssetMenu(fileName = "New Seasonal Tile", menuName = "Tiles/Changing Tile")]
public class ChangingTile : Tile
{
    public Sprite Tile1;
    public Sprite Tile2;
    public Sprite Tile3;
    public Sprite Tile4;

    private Sprite newSprite;

    public override void GetTileData(Vector3Int location, ITilemap tileMap, ref TileData tileData)
    {
        base.GetTileData(location, tileMap, ref tileData);
        GameManager manager = GameManager.GetInstance();
        int tileIndex = 0;
        if(manager != null)
        {
            tileIndex = manager.GetTileTypeIndex();
        }
        else
        {
            GameObject indexSaver = GameObject.Find("MusicManager");
            if(indexSaver != null)
            {
                MenuTileBeatController saver = indexSaver.GetComponent<MenuTileBeatController>();
                tileIndex = saver.tileTypeIndex;
            }
            else
            {
                tileIndex = 0;
            }
        }
        switch (tileIndex)
        {
            case 0:
                newSprite = Tile1;
                break;
            case 1:
                newSprite = Tile2;
                break;
            case 2:
                newSprite = Tile3;
                break;
            case 3:
                newSprite = Tile4;
                break;
        }
        //    Change Sprite
        tileData.sprite = newSprite;
    }
}

