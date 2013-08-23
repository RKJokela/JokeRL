#include "JokeRL.h"
#include "actor.h"


bool useKey()
{
	int dx, dy;
	int locX, locY;
	bool retVal = true;
	player->GetPos(locX, locY);
	message("Which direction?", true);
	getDirection(getch(), dx, dy);
	dx += locX;
	dy += locY;
	int &tile = mapTiles[dy][dx];
	if (tile == TILE_DOOR_LOCKED)
	{
		message("The key turns in the lock...\nThe door is open!");
		tile = TILE_DOOR_OPEN;
	}
	else if (tile == TILE_DOOR_CLOSED)
	{
		message("You lock the door.");
		tile = TILE_DOOR_LOCKED;
	}
	else // invalid direction
	{
		message("Can't use your key there.");
		// no action taken
		retVal = false; 
	}
	return retVal;
}

bool usePick()
{
	int dx, dy;
	char msg[80];
	bool retVal = false;
	TileType afterDigging = TILE_FLOOR_ROCK;
	ItemType debris = ITEM_NONE;

	message("Which direction?", true);
	getDirection(getch(), dx, dy);
	player->GetPos(nPlayerX, nPlayerY);
	dx += nPlayerX;
	dy += nPlayerY;

	int &tile = mapTiles[dy][dx];
	if (catalogTiles[tile].flags & T_DIGGABLE)
	{
		switch (tile)
		{
		case TILE_WALL:
			strcpy(msg, "The rock wall breaks into rubble.");
			debris = ITEM_ROCK;
			break;
		case TILE_TREE:
			strcpy(msg, "You chop the tree down like a true lumberjack.");
			afterDigging = TILE_FLOOR_GRASS;
			break;
		default:
			strcpy(msg, "The pickaxe breaks through the obstruction.");
		}
		tile = afterDigging;
		retVal = true;
		mapItems[dy][dx] = debris;
	}
	else if (catalogTiles[tile].flags & T_PASSABLE)
	{
		strcpy(msg, "You swing your pickaxe awkwardly through the air.");
	}
	else
	{
		strcpy(msg, "Your pickaxe bounces off with a dull THUNK.");
	}

	message(msg);
	return retVal;
}

// swap out an item in your pack for e.g. an item on the ground
// pre: itemToPut = what you're putting in the pack
//		whereToPutIt = index of where it's going
// post: packItems[whereToPutIt] == itemToPut
// return: item previously held in packItems[whereToPutIt]
// ----- DOES NO ERROR CHECKING -----
int swapItemInPack(int itemToPut, int whereToPutIt)
{
	int returnItem = packItems[whereToPutIt];
	packItems[whereToPutIt] = itemToPut;
	return returnItem;
}

// returns 1 on success, 0 on failure
int addItemToPack(ItemType item)
{
	for (int i = 0; i < PLAYER_PACK_SIZE; i++)
	{
		if (!packItems[i])
		{	// put item in pack and remove from ground
			packItems[i] = item;
			return 1;
		}
	}
	// pack is full
	return 0;
}

// 1 for open door, 0 for close door
bool doorCommand(bool open)
{
	int nKey, dx, dy;
	int locX, locY;
	int doorCheck = open ? TILE_DOOR_CLOSED : TILE_DOOR_OPEN;
	int doorSet = open ? TILE_DOOR_OPEN : TILE_DOOR_CLOSED;
	bool retVal = false;
	player->GetPos(locX, locY);

	message("Which direction?", true);
	nKey = getch();

	// Must check this so you can't close a door you're standing on
	if (IS_MOVE_KEY(nKey)) 
	{
		getDirection(nKey, dx, dy);
		locX += dx;
		locY += dy;
		if (mapTiles[locY][locX] == doorCheck)
		{
			mapTiles[locY][locX] = doorSet;
			retVal = true;
		}
		if (open && mapTiles[locY][locX] == TILE_DOOR_LOCKED)
		{
			message("The door is locked!");
		}
	}
	return retVal;
}

bool getCommand()
{
	char strPickedUp[80];
	int locX, locY;
	player->GetPos(locX, locY);
	ItemType itemAtPlayer = (ItemType)mapItems[locY][locX];
	bool retVal = false;
	if (!itemAtPlayer)
	{
		message("There's nothing to pick up here!");
	}
	else if (addItemToPack(itemAtPlayer))
	{	// remove item from the ground
		mapItems[locY][locX] = 0;
		sprintf(strPickedUp, "Picked up a %s", catalogItems[itemAtPlayer].name);
		message(strPickedUp);
		retVal = true;
	}
	else
	{	// no slots are empty
		message("Your pack is full!");
	}
	return retVal;
}

bool dropCommand()
{
	int locX, locY;
	player->GetPos(locX, locY);
	int itemToDrop, &itemOnGnd = mapItems[locY][locX];
	bool retVal = false;
	char strItemDropped[80];

	itemToDrop = promptForPackItem("Drop what? [ABC..]");

	if (itemToDrop < 0)
	{
		message("You can't drop that!");
	}
	else if (itemToDrop != KEY_ESCAPE)
	{	// if standing on an item, it goes in your pack, otherwise pack slot empties
		// I feel clever :P
		itemOnGnd = swapItemInPack(itemOnGnd, itemToDrop);
		sprintf(strItemDropped, "Dropped a %s", catalogItems[itemOnGnd].name);
		message(strItemDropped);
		// successfully dropped it
		retVal = true;
	}
	return retVal;
}

bool useCommand()
{
	int itemToUse = promptForPackItem("Use which item? [ABC..]");
	bool retVal = false;
	if (itemToUse < 0)
		message("You can't use that!");
	else if (itemToUse != KEY_ESCAPE)
	{
		switch (packItems[itemToUse])
		{
		case ITEM_POTION:
			message("You drink the mysterious purple liquid...\nYou feel sexy!\nThe feeling subsides...");
			packItems[itemToUse] = 0;
			break;
		case ITEM_KEY:
			retVal = useKey();
			break;
		case ITEM_PICK:
			retVal = usePick();
			break;
		default:
			message("Nothing happened...");
			break;
		}
	}
	return retVal;
}

bool searchCommand()
{
	// NOT YET IMPLEMENTED - JUST ENDS YOUR TURN
	return true;
}
