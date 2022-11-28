#include "GameState.h"

GameState::GameState() {
	lastmpos = Vector2(0, 0);

	pmspeed = 5;
	prspeed = 0.05;

	pcount = 0;
	pps = new Vector2[MAX_PLAYERS];
	pds = new Vector2[MAX_PLAYERS];
	prs = new int[MAX_PLAYERS];
	pips = new uint32_t[MAX_PLAYERS];

	id = -1;

	lastFirstIt = -1;
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
	bool acting = false;

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
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		acting = true;
	}

	if (pm.length() > 0) {
		pm = pm.unit();
	}

	pp = pp + (pm * (pmspeed * dt));

	PlayerState opponentState;
	if (id != 0) { // if not host
	// implement a debounce so packets arent being sent so quickly
		client.SetPlayerState({pp.x, pp.y, atan2(pp.y, pp.x), acting});
		opponentState = client.GetOpponentState();
	} else {
		server.SetPlayerState({pp.x, pp.y, atan2(pp.y, pp.x), acting});
		opponentState = server.GetOpponentState();
	}
	setPlayerPosition(pp, pd);
	setPlayerPosition(id + 1 % 2, Vector2(opponentState.xPos, opponentState.yPos), Vector2(cos(opponentState.orientation), sin(opponentState.orientation)));
}

void GameState::mouse(double x, double y) {
	double pr = ((lastmpos.x - x) * prspeed);
	pd = pd.rotDeg(pr);
	lastmpos = Vector2(x, y);
}

int GameState::getIdFromIp(uint32_t pip) {
	for (int pid = 0; pid < pcount; pid++) {
		if (pips[pid] == pip) {
			return pip;
		}
	}
	return -1;
}

int GameState::addPlayer(uint32_t ip) {
	if (pcount < MAX_PLAYERS) {
		prs[pcount] = 0; // not in game
		pips[pcount] = ip;
		pcount++;
		return pcount - 1;
	}
	return -1;
}

void GameState::startGame() {
	std::cout << "\n" << id << "\n";
	if (id == 0) { // if host
		for (int i = 0; i < pcount; i++) { // for every player
			prs[i] = 1; // not it;
			pps[i] = NOT_IT_START;
			pds[i] = Vector2(0, 1).rotDeg(0.001f);
		}

		srand(3); // 2: host starts as not it. 3: host starts as it.

		int it;
		if (pcount == 1) {
			it = 0;
		}
		else if (lastFirstIt < 0) {
			it = rand() % pcount;
		}
		else {
			it = rand() % pcount - 1;
			if (it >= lastFirstIt) {
				it++;
			}
		}
		prs[it] = 2; // it
		pps[it] = IT_START;
	}
}

bool GameState::endGame() { // returns true if every player is either it or not in game. also returns false if only 1 player is in the game
	if (id == 0) { // if host
		if (pcount == 1) {
			return false;
		}
		for (int i = 0; i < pcount; i++) { //for every player
			if (prs[i] == 1) {
				return false;
			}
		}
		return true;
	}
}

void GameState::setLocalPlayerPosition() {
	if (id >= 0) {
		pp = pps[id];
		pd = pds[id];
		pm = Vector2(0, 0);
	}
}

void GameState::setPlayerPosition(int pid, Vector2 newpp, Vector2 newpd) {
	pps[pid] = newpp;
	pds[pid] = newpd;
}

void GameState::setPlayerPosition(uint32_t pip, Vector2 newpp, Vector2 newpd) {
	setPlayerPosition(getIdFromIp(pip), newpp, newpd);
}

void GameState::setPlayerPosition(Vector2 newpp, Vector2 newpd) {
	if (id == 0) { // if host
		setPlayerPosition(0, newpp, newpd);
	} else {
		// send a packet to the host
	}
}

void GameState::act(int pid) {
	for (int i = 0; i < pcount; i++) {
		if (i != pid) {
			if ((pps[i] - pps[pid]).lengthSquared() < TAG_DISTANCE) {
				prs[i] = 2; // set role to It
				std::cout << "tagged!" << "\n";
			} else {
				std::cout << (pps[i] - pps[pid]).lengthSquared() << "\n";
			}
		}
	}
}

void GameState::act(uint32_t pip) {
	act(getIdFromIp(pip));
}

void GameState::act() {
	if (id == 0) { // if host
		act(0);
	} else {
		// send a packet to the host
	}
}