#include <ctime>
#include "JokeRL.h"
#include "actor.h"

#define DEBUGMODE

chtype playerCChar = CHAR_PLAYER | COLOR_PLAYER;
bool quit = false;
int nPlayerX = 4, nPlayerY = 4;

int main(int argc, char *argv[])
{
	// declarations
	time_t currentTime = 0;
	int nKey = 0;
	int openOrClose = 1; // open door
	bool messageIsOld = false;
	bool playerTurnEnded = false;

	// initialization
	initscr();
	curs_set(false);
	noecho();
	keypad(stdscr, true);
	start_color();
	// set 256 colors - all colorpairs have black background for now
	initColors();
	// zero out items on map
	initItems();
	initActors();

	// testing out items, actors, etc.
	placeItem(12, 4, ITEM_POTION);
	placeItem(4, 3, ITEM_KEY);
	placeItem(6, 9, ITEM_PICK);

	placeActor(3, 3, '&', COLOR_PAIR(COLOR_RED));
	placeActor(1, 13, '&', COLOR_PAIR(COLOR_CYAN));

	message("Welcome to JokeRL!");
	currentTime = time(nullptr);
	// seed RNG
	srand(currentTime);

	// main loop
	while (!quit)
	{
		// OUTPUT

		clear();
		// draw map and items layers
		drawMap();
		// draw ACTORS layer
		drawActors();
		// show message
		if (*msgBuf) 
			message(nullptr);
		// display pack contents
		showInventory();
		// display the time
		mvaddstr(0, 0, ctime(&currentTime));

		napms(1); // to update the screen

		// INPUT

		nKey = getch();

		// PROCESSING

		currentTime = time(nullptr);

		// reset turn flag
		playerTurnEnded = false;

		if (IS_MOVE_KEY(nKey))
		{
			playerTurnEnded = movePlayer(nKey);
		}
		else switch (nKey)
		{
		case KEY_QUIT:
			quit = true;
			break;
		case KEY_CLOSE:
			openOrClose = 0; // close the door
		case KEY_OPEN:
			playerTurnEnded = doorCommand(openOrClose);
			openOrClose = 1; // reset variable!
			break;
		case KEY_GET:
			playerTurnEnded = getCommand();
			break;
		case KEY_DROP:
			playerTurnEnded = dropCommand();
			break;
		case KEY_USE:
		case KEY_APPLY:
			playerTurnEnded = useCommand();
			break;
		case KEY_SPAWN_ACTOR:
			placeActor(1, 13, '&', COLOR_PAIR(COLOR_MAGENTA));
			break;
		case KEY_KILL_ACTOR:
			removeActor(listActors[lastActorUsed]);
			break;
		case KEY_SEARCH:
			playerTurnEnded = searchCommand();
		default:
			break;
		}
		// move npc's only if player has taken a turn
		if (playerTurnEnded)
		{
			updateActors();
		}
	}

	return endwin();
}

bool movePlayer(int nKey)
{
	int dx, dy;

	getDirection(nKey, dx, dy);

	return player->Move(dx, dy);
}

void initColors()
{
	Uint8 r, g, b;
	if (!can_change_color())
		return;
	for (int i = 1; i < COLOR_PAIRS; i++)
	{
		if (i > 15)
		{
			r = ((i - 15)/36);
			g = (((i - 15)%36)/6);
			b = ((i - 15)%6);
			// scale to 255
			r *= 51; g *= 51; b *= 51;
			init_color(i, r, g, b);
		}
		init_pair(i, i, COLOR_BLACK);
	}
}

void testColors()
{
	chtype toDraw, ch = 0xdb;
	int ind;
	move(0, 0);
	for (ind = 0; ind < COLOR_PAIRS; ind++)
	{
		if (ind%16 == 0)
			addch('\n');
		toDraw = ch | COLOR_PAIR(ind);
		addch(toDraw);
	}
}
