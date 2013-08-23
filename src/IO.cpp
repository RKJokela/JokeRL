#include "JokeRL.h"

char msgBuf[80];

void getDirection(int nKey, int &dx, int &dy)
{
	int negX = 1, negY = 1;
	dx = 0;
	dy = 0;

	// check horizontal movement
	switch (nKey)
	{
	case KEY_LEFT:
	case KEY_UPLEFT:
	case KEY_DOWNLEFT:
		negX = -1;
	case KEY_RIGHT:
	case KEY_UPRIGHT:
	case KEY_DOWNRIGHT:
		dx = negX;
	}
	
	// check vertical movement
	switch (nKey)
	{
	case KEY_UP:
	case KEY_UPLEFT:
	case KEY_UPRIGHT:
		negY = -1;
	case KEY_DOWN:
	case KEY_DOWNLEFT:
	case KEY_DOWNRIGHT:
		dy = negY;
	}
}

// SIMPLE TEMPORARY MESSAGE ROUTINE
// When you call this by passing a string, it will be stored in the
// message buffer until the next draw cycle. The message will be shown
// for one turn, then disappear or be replaced with new message.
void message(char* msg, bool immediate)
{
	if (immediate)
	{	// write message now! need this for mid-turn prompts like open/close etc.
		// skips the buffer & will be erased at beginning of next turn
		move(MESSAGE_POS_Y, MESSAGE_POS_X);
		deleteln(); deleteln(); deleteln();
		insertln(); insertln(); insertln();
		addstr(msg);
		napms(1);
	}
	else if (msg == nullptr)
	{	// PRINTS TO SCREEN - ONLY CALLED FROM MAIN LOOP
		mvaddstr(MESSAGE_POS_Y, MESSAGE_POS_X, msgBuf);
		// clear message
		msgBuf[0] = '\0';
	}
	else
	{
		strncpy(msgBuf, msg, 80);
	}
}

// returns -1 if not valid pack location
// returns KEY_ESCAPE if user hit escape to cancel prompt
// otherwise, returns index into packItems
// if emptyOK, can select empty slots (default false)
int promptForPackItem(char* msg, bool emptyOK)
{
	int nKey;
	message(msg, true);
	nKey = getch();
	if (nKey != KEY_ESCAPE)
	{
		// accept upper or lower case (convert anything to upper)
		nKey = toupper(nKey) - 'A';
		if (nKey < 0 || nKey >= PLAYER_PACK_SIZE || (!emptyOK && packItems[nKey] == 0))
			nKey = -1;
	}
	return nKey;
}

void showInventory()
{
	char invLetter;
	int i;
	int posX = INVENT_POS_X;
	int posY = INVENT_POS_Y;
	mvaddstr(posY,		posX, "INVENTORY");
	mvaddstr(posY + 1,	posX, "---------");
	for (i = 0, invLetter = 'A'; i < PLAYER_PACK_SIZE; i++, invLetter++)
	{
		move(posY + 2 + i, posX);
		addch(invLetter);
		addstr(": ");
		addstr(catalogItems[packItems[i]].name);
	}
}
