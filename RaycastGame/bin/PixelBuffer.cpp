#include "PixelBuffer.h"

PixelBuffer::PixelBuffer(int a, int b, int c) {
	x = a;
	y = b;
	z = c;
	i = 0, o = 0, p = 0, q = 0, r = 0;
	buff = new unsigned char[(long long int) y * x * z];
	rgbBuff = new uint32_t[y * x];
}

PixelBuffer::PixelBuffer() {
	PixelBuffer::PixelBuffer(1, 1, 3);
}

void PixelBuffer::operate(std::function<uint32_t (int, int)> func, int a1, int b1, int a2, int b2) {
	for (r = b1; r < b2; r++) { // for(r = b1 > 0 ? b1 : 0; r < (b2 < y ? b2 : y); r++) {
		for(q = a1; q < a2; q++) { // for (q = a1 > 0 ? a1 : 0; q < (a2 < x ? a2 : x); q++) {
			set(q,r,func(q, r));
			//set(q, r, func(q, r));
		}
	}
}
void PixelBuffer::operate(std::function<uint32_t(int, int)> func, Vector2 start, Vector2 end) {
	operate(func, start.x, start.y, end.x, end.y);
}
void PixelBuffer::operate(std::function<uint32_t(int, int)> func) {
	operate(func, 0, 0, x - 1, y - 1);
}

uint32_t PixelBuffer::get(int a, int b) {
	return rgbBuff[a * x + b];
	/*
	o = 0;
	for (i = 0; i < z; i++) {
		o |= buff[b * x * z + a * z + i] << (8 * (3 - i));
	}
	return o;
	*/
}

void PixelBuffer::set(int a, int b, uint32_t v) {
	if ((v & 0x000000FF) < 0xFF) {
		return; // if alpha is less than 0xFF, ignore the pixel. todo: add alpha compositing
	}
	rgbBuff[b * x + a] = v;
	/*
	for(i = 0; i < z; i++) {
		buff[b * x * z + a * z + i] = (v >> (8 * (3 - i))) & 0xFF;
	}
	*/
}

void PixelBuffer::clear() {
	operate([](int x, int y) {return 0x11111111;});
}

void PixelBuffer::drawBuffer(Vector2 pos) {
	for (int i = 0; i < x * y; i++) {
		for (int o = 0; o < z; o++) {
			buff[i * z + o] = (rgbBuff[i] >> (8 * (3 - o))) & 0xFF;
		}
	}
	glRasterPos2f(pos.x, pos.y);
	glDrawPixels(x, y, (z > 3) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, buff);
}