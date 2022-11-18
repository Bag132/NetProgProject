#include "Raycaster.h"
#include <iostream>

int texturePositions[2] = { 0, 0 };

uint32_t textures[256] = {
	// index 1, position 0: 16x16 stone wall
	0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x404040FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x404040FF, 0x808080FF, 0x808080FF, 0x808080FF,
	0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x404040FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x404040FF, 0x808080FF, 0x808080FF, 0x808080FF,
	0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x404040FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x404040FF, 0x808080FF, 0x808080FF, 0x808080FF,
	0x404040FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x808080FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x808080FF, 0x404040FF, 0x404040FF, 0x404040FF,
	0x404040FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x404040FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF,
	0x404040FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x404040FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF,
	0x404040FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x404040FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF,
	0x808080FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x808080FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x404040FF,
	0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x404040FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x404040FF, 0x808080FF, 0x808080FF, 0x808080FF,
	0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x404040FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x404040FF, 0x808080FF, 0x808080FF, 0x808080FF,
	0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x404040FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x404040FF, 0x808080FF, 0x808080FF, 0x808080FF,
	0x404040FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x808080FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x808080FF, 0x404040FF, 0x404040FF, 0x404040FF,
	0x404040FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x404040FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF,
	0x404040FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x404040FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF,
	0x404040FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x404040FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF, 0x808080FF,
	0x808080FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x808080FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x404040FF, 0x404040FF
};

// dimColors[i] = int(i * 0.75)
// precomputing these improves performance
uint32_t dimColors[256] = { 0, 0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 18, 19, 20, 21, 21, 22, 23, 24, 24, 25, 26, 27, 27, 28, 29, 30, 30, 31, 32, 33, 33, 34, 35, 36, 36, 37, 38, 39, 39, 40, 41, 42, 42, 43, 44, 45, 45, 46, 47, 48, 48, 49, 50, 51, 51, 52, 53, 54, 54, 55, 56, 57, 57, 58, 59, 60, 60, 61, 62, 63, 63, 64, 65, 66, 66, 67, 68, 69, 69, 70, 71, 72, 72, 73, 74, 75, 75, 76, 77, 78, 78, 79, 80, 81, 81, 82, 83, 84, 84, 85, 86, 87, 87, 88, 89, 90, 90, 91, 92, 93, 93, 94, 95, 96, 96, 97, 98, 99, 99, 100, 101, 102, 102, 103, 104, 105, 105, 106, 107, 108, 108, 109, 110, 111, 111, 112, 113, 114, 114, 115, 116, 117, 117, 118, 119, 120, 120, 121, 122, 123, 123, 124, 125, 126, 126, 127, 128, 129, 129, 130, 131, 132, 132, 133, 134, 135, 135, 136, 137, 138, 138, 139, 140, 141, 141, 142, 143, 144, 144, 145, 146, 147, 147, 148, 149, 150, 150, 151, 152, 153, 153, 154, 155, 156, 156, 157, 158, 159, 159, 160, 161, 162, 162, 163, 164, 165, 165, 166, 167, 168, 168, 169, 170, 171, 171, 172, 173, 174, 174, 175, 176, 177, 177, 178, 179, 180, 180, 181, 182, 183, 183, 184, 185, 186, 186, 187, 188, 189, 189, 190 };


Raycaster::Raycaster(GameState* _state, int sizx, int sizy, int res, bool rgba) : Raycaster::Topdown(_state, sizx, sizy, res, rgba) {
	fov = 90;
};

void Raycaster::drawRaycaster(Vector2 pos) {
	pbuff.clear();
	Vector2 plane = state->pd.rotDeg(-90) * (fov / 90);
	// floor/ceiling rendering code
	for (int y = 0; y < pbuff.y / 2; y++) {
		Vector2 dir0 = state->pd - plane;
		Vector2 dir1 = state->pd + plane;
		float rowDist = (0.5 * pbuff.y) / (y - 0.5 * pbuff.y);
		Vector2 step = plane * (2 * rowDist / pbuff.x);
		Vector2 floor = state->pp + Vector2(100,100) + (dir0 * rowDist); // not exactly sure why adding (100,100) makes this work but it does, somehow

		pbuff.operate([&](int x, int y) {
			int texX = int(texSize * (x * step.x + floor.x)) % texSize;
			int texY = int(texSize * (x * step.y + floor.y)) % texSize;

			return textures[texturePositions[floorTex] + texY * texSize + texX];
		}, 0, y, pbuff.x, y + 1);

		pbuff.operate([&](int x, int y) {
			int texX = int(texSize * (x * step.x + floor.x)) % texSize;
			int texY = int(texSize * (x * step.y + floor.y)) % texSize;

			return textures[texturePositions[floorTex] + texY * texSize + texX];
		}, 0, pbuff.y - y - 1, pbuff.x, pbuff.y - y);

		/*
		pbuff.operate([&](int x, int y) {
			int texX = int(texSize * (floor.x - int(floor.x))) % texSize;
			int texY = int(texSize * (floor.y - int(floor.y)));

			return textures[texturePositions[ceilTex] + int(texSize * (floor.y - int(floor.y))) * int(texSize * (floor.x - int(floor.x)))];
			}, 0, pbuff.y - y - 1, pbuff.x, pbuff.y - y);
		*/
	}

	// wall rendering code
	//for (int x = 0; x < pbuff.x; x++) {
	for (int x = 0; x < pbuff.x; x++) {
		bool north;
		Vector2 step;
		Vector2 dir = (state->pd + plane * (2.0f * x / (float)pbuff.x - 1.0f));
		step.x = (dir.x > 0.0) ? 1.0f : -1.0f;
		step.y = (dir.y > 0.0) ? 1.0f : -1.0f;
		Vector2 hitx, hity;
		int tilex = 0, tiley = 0;
		if (dir.y != 0) {
			for (int a = step.y == 1 ? 1 : 0; a < 1000; a++) {
				hity = dir * (((int)state->pp.y + step.y * (a + 0.00001f)) - state->pp.y) / dir.y;
				if ((state->pp + hity).oob(Vector2(sizx, sizy))) {
					hity = Vector2(INFINITY, INFINITY);
					//std::cout << "x ";
					break;
				}
				else {
					tiley = map[(int)(((state->pp + hity).flr().y) * (float)sizx + (state->pp + hity).flr().x)];
					if (tiley > 0) {
						//std::cout << hity.length() << " ";
						break;
					}
				}
			}
		} else {
			hity = Vector2(INFINITY, INFINITY);
		}
		if (dir.x != 0) {
			for (int b = step.x == 1 ? 1 : 0; b < 1000; b++) {
				hitx = dir * (((int)state->pp.x + step.x * (b + 0.00001f)) - state->pp.x) / dir.x;
				if ((state->pp + hitx).oob(Vector2(sizx, sizy))) {
					hitx = Vector2(INFINITY, INFINITY);
					//std::cout << "x\n";
					break;
				}
				else {
					tilex = map[(int)((state->pp + hitx).flr().y * (float)sizx + (state->pp + hitx).flr().x)];
					if (tilex > 0) {
						//std::cout << hitx.length() << '\n';
						break;
					}
				}
			}
		} else {
			hitx = Vector2(INFINITY, INFINITY);
		}
		north = hity.length() < hitx.length();
		float d = fmin(hitx.length(), hity.length()) / dir.length();
		int lineHeight = (int)(pbuff.y / d);
		int drawStart = -lineHeight / 2 + pbuff.y / 2;
		int drawEnd = lineHeight / 2 + pbuff.y / 2;

		int tile = north ? tiley : tilex;
		float wallx;
		if (north) {
			wallx = state->pp.x + d * dir.x;
		} else {
			wallx = state->pp.y + d * dir.y;
		}
		int texY, texX = int(wallx * double(texSize)) % texSize;
		if ((north && dir.x > 0) || (!north && dir.x < 0)) {
			texX = texSize - texX - 1;
		}

		double texStep = double(texSize) / lineHeight;
		double texPos = 0;
		pbuff.operate([&](int x, int y) {
			uint32_t color = textures[texturePositions[tile] + int(texStep * (drawEnd - y)) * texSize + texX];
			if (!north) {
				color = (dimColors[(color & 0xFF000000) >> 24] << 24) | (dimColors[(color & 0x00FF0000) >> 16] << 16) | (dimColors[(color & 0x0000FF00) >> 8] << 8) | (color & 0x000000FF); // each color channel is multiplied by 0.9. dimColors[i] = i * 0.75.
			}
			return color;

		}, x, drawStart < 0 ? 0 : drawStart, x + 1, drawEnd > pbuff.y ? pbuff.y : drawEnd);
	}
	pbuff.drawBuffer(pos);
}
