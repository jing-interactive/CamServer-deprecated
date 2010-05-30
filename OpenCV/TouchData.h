#ifndef __TOUCHLIB_TOUCHDATA__
#define __TOUCHLIB_TOUCHDATA__

struct TouchData
{
	int ID;
	int tagID;
	float X;
	float Y;

	float height;
	float width;
	float angle;

	float area;

	float dX;
	float dY;

};

#endif
