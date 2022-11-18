#pragma once

#include "Topdown.h"
#include <iostream>

class Raycaster : public Topdown {
public:
	int fov;

	const int texSize = 16;

	const int floorTex = 0;
	const int ceilTex = 0;

	Raycaster(GameState* _state, int sizx, int sizy, int res, bool rgba);

	void drawRaycaster(Vector2 pos);
};