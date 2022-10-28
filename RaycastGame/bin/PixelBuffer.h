#pragma once

#include <GLFW/glfw3.h>
#include <functional>
#include <iostream>
#include <array>

#include "Vector2.h"

class PixelBuffer
{
public:
	int x, y, z, q, r;
	unsigned char* buff;
	uint8_t i;
	uint32_t o;

	PixelBuffer(int a, int b, int c);
	PixelBuffer();

	void operate(std::function<uint32_t(int, int)> func, int a1, int b1, int a2, int b2);
	void operate(std::function<uint32_t(int, int)> func, Vector2 start, Vector2 end);
	void operate(std::function<uint32_t(int, int)> func);
	uint32_t get(int a, int b);
	void set(int a, int b, uint32_t v);
	void clear();
	void drawBuffer(Vector2 pos);
};
