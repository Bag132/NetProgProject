#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "GameState.h"
#include "PixelBuffer.h"

class Topdown {
public:
	GameState *state;
	PixelBuffer pbuff;

	int sizx, sizy, scale;

	Topdown::Topdown(GameState* _state, int sizx, int sizy, int res, bool rgba); //I don't understand this warning. lmao it works fine
	Topdown::Topdown() = default;
	void drawTopdown(Vector2 pos);
};