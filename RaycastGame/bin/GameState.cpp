#include "GameState.h"

GameState::GameState(Vector2 _pp) {
	pp = _pp;
	pd = Vector2(0, 1).rotDeg(0.001f);
	pm = Vector2(0, 0);
	lastmpos = Vector2(0, 0);

//	std::cout << "Changed PD!";

	pmspeed = 5;
	prspeed = 0.05;
}
GameState::GameState() {
	GameState(Vector2(0, 0));
}

void GameState::buttons(int key, int scancode, int action, int mods) {
	/*
	if (action == GLFW_PRESS) {
		switch (key) {
		case 265:
			pm = pm + pd;
			break;
		case 264:
			pm = pm - pd;
			break;
		case 263:
			//pp = pp - Vector2(1, 0);
			pd = pd.rotDeg(5);
			break;
		case 262:
			//pp = pp + Vector2(1, 0);
			pd = pd.rotDeg(-5);
			break;
		}
	}
	*/
}

void GameState::input(GLFWwindow* window, double dt) {
	pm = Vector2(0, 0);

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		pm = pm + pd;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		pm = pm - pd;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		pm = pm + pd.rotDeg(90);
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		pm = pm + pd.rotDeg(-90);
	}

	if (pm.length() > 0) {
		pm = pm.unit();
	}
	pp = pp + (pm * (pmspeed * dt));
}

void GameState::mouse(double x, double y) {
	double pr = ((lastmpos.x - x) * prspeed);
	pd = pd.rotDeg(pr);
	lastmpos = Vector2(x, y);
}
