#include "JokeRL.h"
#include "actor.h"

// dungeon.cpp
//	Contains data for map, items, & monsters

MapTile catalogTiles[TOTAL_TILES] = {
	{ ' ', false },	// TILE_EMPTY = 0,
	{ CHAR_WALL			| COLOR_WALL,	T_DIGGABLE	},	// TILE_WALL,
	{ CHAR_FLOOR		| COLOR_ROCK,	T_PASSABLE	},	// TILE_FLOOR_ROCK,
	{ CHAR_FLOOR		| COLOR_GRASS,	T_PASSABLE	},	// TILE_FLOOR_GRASS,
	{ CHAR_TREE			| COLOR_TREE,	T_DIGGABLE	},	// TILE_TREE,
	{ CHAR_DOOR_OPEN	| COLOR_DOOR,	T_PASSABLE	},	// TILE_DOOR_OPEN,
	{ CHAR_DOOR_CLOSED	| COLOR_DOOR,	0			},	// TILE_DOOR_CLOSED,
	{ CHAR_DOOR_CLOSED	| COLOR_DOOR,	0			}	// TILE_DOOR_LOCKED,
};

Item catalogItems[TOTAL_ITEMS] = {
	{ ' ',							"Empty"		},	// ITEM_NONE = 0,
	{ CHAR_POTION	| COLOR_POTION,	"potion"	},	// ITEM_POTION,
	{ CHAR_ROCK		| COLOR_ROCK,	"rock"		},	// ITEM_ROCK,
	{ CHAR_KEY		| COLOR_KEY,	"key"		},	// ITEM_KEY,
	{ CHAR_PICK		| COLOR_PICK,	"pickaxe"	}	// ITEM_PICK,

};

int packItems[PLAYER_PACK_SIZE];
int mapItems[MAP_HEIGHT][MAP_WIDTH];
int mapTiles[MAP_HEIGHT][MAP_WIDTH] = {
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1 },
	{ 1, 3, 1, 1, 1, 1, 1, 3, 3, 3, 1, 1, 1, 1, 1, 3, 3, 3, 3, 1 },
	{ 1, 3, 1, 2, 2, 2, 1, 3, 3, 3, 1, 2, 2, 2, 1, 3, 3, 3, 3, 1 },
	{ 1, 3, 1, 2, 2, 2, 1, 3, 3, 3, 1, 2, 2, 2, 1, 3, 3, 3, 3, 1 },
	{ 1, 3, 1, 2, 2, 2, 1, 3, 3, 3, 1, 2, 2, 2, 1, 3, 3, 3, 3, 1 },
	{ 1, 3, 1, 1, 7, 1, 1, 3, 3, 3, 1, 1, 7, 1, 1, 3, 3, 3, 3, 1 },
	{ 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1 },
	{ 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1 },
	{ 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1 },
	{ 1, 3, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1 },
	{ 1, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1 },
	{ 1, 3, 3, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1 },
	{ 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1 },
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};
Actor* listActors[MAX_ACTORS];
Actor* &player = listActors[0];
int lastActorUsed;

void drawMap()
{
	for (int y = 0; y < MAP_HEIGHT; y++)
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{			
			drawTile(x, y);
		}
	}
}

void drawTile(int x, int y)
{
	// pick item char if exists, otherwise dungeon char
	chtype drawCh = mapItems[y][x]  ? catalogItems[mapItems[y][x]].colorChar
									: catalogTiles[mapTiles[y][x]].colorChar;
	mvaddch(y + MAP_OFFSET_Y, x + MAP_OFFSET_X, drawCh);
}

// passable cells are any cell that has the T_PASSABLE flag AND doesn't already contain an actor
bool isPassable(int x, int y)
{
	return ( x >= 0 && x < MAP_WIDTH
		  && y >= 0 && y < MAP_HEIGHT
		  && catalogTiles[mapTiles[y][x]].flags & T_PASSABLE
		  && !actorAtLoc(x, y));
}

// zero out the grid
void initItems()
{
	memset(mapItems, 0, sizeof(mapItems));
	memset(packItems, 0, sizeof(packItems));
}

// initialize player and array of null
void initActors()
{
	memset(listActors, (int)nullptr, sizeof(listActors));
	// listActors[0] is always the player!
	player = new Actor(START_LOC_X, START_LOC_Y, CHAR_PLAYER, COLOR_PLAYER, 0);
	lastActorUsed = 0;
}

// does no checking for existing item there!
void placeItem(int x, int y, ItemType item)
{
	mapItems[y][x] = item;
}

void updateActors()
{
	for (int i = 1; i <= lastActorUsed; i++)
	{
		listActors[i]->Update();
	}
}

// creates actor and puts it on the map
// returns ptr to created actor
Actor* placeActor(int x, int y, char displayChar, chtype color)
{
	int i;
	Actor* newActor;
	// step thru the list till you find an empty spot
	for (i = 1; listActors[i] != nullptr && i < MAX_ACTORS; i++) {}
	if (i == MAX_ACTORS) // list full!
		return nullptr;
	else
	{
		// put an actor in that spot
		// if there were gaps in the list, it goes in the first gap
		// if not, it goes at the end and increments lastActorUsed
		newActor = new Actor(x, y, displayChar, color, i);
		listActors[i] = newActor;
		if (i > lastActorUsed)
			lastActorUsed = i;
		return newActor;
	}
}

// returns true on success, false if actor is null or wasn't in the list
bool removeActor(Actor* toRemove)
{
	int index;
	bool retVal = false;
	if (toRemove != nullptr)
	{
		index = toRemove->GetID();
		if (index > 0 && index < MAX_ACTORS && listActors[index] == toRemove)
		{	// can't remove player!
			listActors[index] = nullptr;
			delete toRemove;
			if (index == lastActorUsed)
			{
				// step back to the last filled entry in the list
				for (lastActorUsed -= 1; listActors[lastActorUsed] == nullptr; lastActorUsed--) { }
			}
			retVal = true;
		}
	}
	return retVal;
}

// for convenience
bool removeActorAt(int x, int y)
{
	return removeActor(actorAtLoc(x, y));
}

void drawActors()
{
	// start at the end of the list and go backward
	// that way, player is drawn last (so will always be shown)
	for (int i = lastActorUsed; i >= 0; i--)
	{
		if (listActors[i] != nullptr)
		{
			listActors[i]->Draw();
		}
	}
}

Actor* actorAtLoc(int x, int y)
{
	int locX, locY;
	for (int i = 0; i <= lastActorUsed; i++)
	{

		if (listActors[i] != nullptr)
		{
			listActors[i]->GetPos(locX, locY);
			if (x == locX && y == locY)
				return listActors[i];
		}
	}
	return nullptr;
}
