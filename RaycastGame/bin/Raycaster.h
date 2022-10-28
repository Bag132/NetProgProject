#pragma once

#include "Topdown.h"

class Raycaster : public Topdown {
public:
	int fov;

	Raycaster(GameState* _state, int sizx, int sizy, int res, bool rgba);

	void drawRaycaster(Vector2 pos);
};