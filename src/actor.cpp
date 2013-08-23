#include "actor.h"

Actor::Actor() : _x(0), _y(0), _colorChar('A'), _id(-1) { }

Actor::Actor(int x, int y, char displayChar, chtype color, const int id) 
	: _x(x), _y(y), _colorChar((chtype)displayChar | color), _id(id) { }

Actor::~Actor() { }

void Actor::GetPos(int &x, int &y) const
{
	x = _x;
	y = _y;
}

void Actor::SetAppearance(char displayChar, int r, int g, int b)
{
	_colorChar = COLOR_CHAR(displayChar, r, g, b);
}

void Actor::SetAppearance(char displayChar, chtype color)
{
	_colorChar = (chtype)displayChar | color;
}

void Actor::SetPos(int x, int y)
{
	_x = CLAMP(x, 0, MAP_WIDTH - 1);
	_y = CLAMP(y, 0, MAP_HEIGHT - 1);
}

void Actor::Update()
{
	// move around randomly
	int dx, dy, nKey;
	do {
		nKey = rand();
		nKey = nKey % 9 + KEY_UPLEFT;
		getDirection(nKey, dx, dy);
	} while (!Move(dx, dy));
}

void Actor::Draw() const
{
	mvaddch(_y + MAP_OFFSET_Y, _x + MAP_OFFSET_X, _colorChar);
}

bool Actor::Move(int dx, int dy)
{
	if (dx == 0 && dy == 0) // standing still is a valid move!
		return true;
	int x = _x + dx;
	int y = _y + dy;
	if (isPassable(x, y))
	{
		_x = x;
		_y = y;
		return true;
	}
	return false;
}

int Actor::GetID() const { return _id; }
