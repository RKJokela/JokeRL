#ifndef		ACTOR_H
#define		ACTOR_H

#include "JokeRL.h"

class Actor
{
public:
	// default constructor
	Actor();

	// set location and display
	Actor::Actor(int x, int y, char displayChar, chtype color, int id);

	// destructor
	~Actor();

	void GetPos(int &x, int &y) const;

	// set character and color (rgb values are 0..5)
	void SetAppearance(char displayChar, int r, int g, int b);
	void SetAppearance(char displayChar, chtype color);

	// set position on map
	void SetPos(int x, int y);
	
	// move to a new location; returns true if success
	bool Move(int dx, int dy);

	// tell the actor to do its thing
	void Update();

	// tell the actor to draw itself
	void Draw() const;

	// get its ID number
	int GetID() const;

protected:
	// coords relative to dungeon origin
	int _x;
	int _y;

	// holds its index into actors list
	const int _id;

	// character displayed
	chtype _colorChar;
};

#endif	//	ACTOR_H
