#include "Raycaster.h"
#include <iostream>

Raycaster::Raycaster(GameState* _state, int sizx, int sizy, int res, bool rgba) : Raycaster::Topdown(_state, sizx, sizy, res, rgba) {
	fov = 90;
};

void Raycaster::drawRaycaster(Vector2 pos) {
	pbuff.clear();
	Vector2 plane = state->pd.rotDeg(-90) * (fov / 90);
	//for (int x = 0; x < pbuff.x; x++) {
	for (int x = 0; x < pbuff.x; x++) {
		bool north;
		Vector2 step;
		Vector2 dir = (state->pd + plane * (2.0f * x / (float)pbuff.x - 1.0f));
		step.x = (dir.x > 0.0) ? 1.0f : -1.0f;
		step.y = (dir.y > 0.0) ? 1.0f : -1.0f;
		Vector2 hitx, hity;
		if (dir.y != 0) {
			for (int a = step.y == 1 ? 1 : 0; a < 1000; a++) {
				hity = dir * (((int)state->pp.y + step.y * (a + 0.00001f)) - state->pp.y) / dir.y;
				if ((state->pp + hity).oob(Vector2(sizx, sizy))) {
					hity = Vector2(INFINITY, INFINITY);
					//std::cout << "x ";
					break;
				}
				else if (map[(int)(((state->pp + hity).flr().y) * (float)sizx + (state->pp + hity).flr().x)] > 0) {
					//std::cout << hity.length() << " ";
					break;
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
				else if (map[(int)((state->pp + hitx).flr().y * (float)sizx + (state->pp + hitx).flr().x)] > 0) {
					//std::cout << hitx.length() << '\n';
					break;
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
		pbuff.operate([&](int x, int y) {
			return north ? 0xffffffff : 0xddddddff;
		}, x, drawStart < 0 ? 0 : drawStart, x + 1, drawEnd > pbuff.y ? pbuff.y : drawEnd);
	}
	pbuff.drawBuffer(pos);
}
