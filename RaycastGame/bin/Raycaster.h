#pragma once

#include "Topdown.h"
#include <iostream>
#include <algorithm>

class Raycaster : public Topdown {
public:
	int fov;

	int* texturePositions;
	uint32_t* textures;
	uint32_t* dimColors;

	const int texSize = 16;
	const int spriteSize = 16;

	const int floorTex = 3;
	const int ceilTex = 4;
	const int plrNotItTex = 5;
	const int plrItTex = 6;

	Raycaster(GameState* _state, int sizx, int sizy, int res, bool rgba);

	void drawRaycaster(Vector2 pos);
};