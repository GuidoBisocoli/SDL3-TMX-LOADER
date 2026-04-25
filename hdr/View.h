#pragma once

#include "Globals.h"

// Simple View Class for scrolling
class View
{
public:
	View(int x, int y, int width, int height, bool pp = true)
	{
		view.x = x;
		view.y = y;
		view.w = width;
		view.h = height;
		pixelPerfect = pp;
	}
	~View() {}

	SDL_FRect getRect() { return view; }

	void update(float x, float y) {
		view.x += x;
		view.y += y;
		if (pixelPerfect)
			pixelPerfectCorrection();
	}
private:
	SDL_FRect view;
	bool pixelPerfect;

	// Round to the nearest pixel
	void pixelPerfectCorrection()
	{
		view.x = round(view.x);
		view.y = round(view.y);
	}
};