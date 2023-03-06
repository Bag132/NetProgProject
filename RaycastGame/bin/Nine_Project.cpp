// Nine_Project.cpp : Defines the entry point for the application.
//

#include "Nine_Project.h"
#include "Comms.h"
#include <thread>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

GLFWwindow* window;
GameState gamestate;

const int RES_X = 800;
const int RES_Y = 300;
const int SIZE_X = 40;
const int SIZE_Y = 30;
const int SCALE = 10;

int t0 = 0, t1;
double dt;
int seconds = 0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

GLFWwindow* initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(RES_X, RES_Y, "Nine_Project", NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		throw std::runtime_error("Failed to create GLFW window");
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		throw std::runtime_error("Failed to initalize GLAD");
	}
	glViewport(0, 0, RES_X, RES_Y);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	return window;
}

int renderLoop(GLFWwindow* window, Raycaster raycaster) {
	//glDrawBuffer(GL_BACK);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//glBindBuffer(GL_PIXEL_UNPACK_BUFFER, GL_BACK);

	while (!glfwWindowShouldClose(window)) {
		t1 = clock();
		dt = ((long long int) t1 - t0) / (double) CLOCKS_PER_SEC;
		t0 = t1;

		gamestate.input(window, dt);

		// rendering commands here
		glClear(GL_COLOR_BUFFER_BIT); //| GL_DEPTH_BUFFER_BIT);

		raycaster.drawTopdown(Vector2(0, -1));
		raycaster.drawRaycaster(Vector2(-1, -1));

		//glFlush();
		glfwSwapBuffers(window);
		glfwPollEvents();
		
		if (clock() / (float)CLOCKS_PER_SEC - 1 > seconds) {
			seconds++;
			std::cout << 1.0 / dt << '\n';
		}
	}
	glfwTerminate();
	return 0;
}

void input(GLFWwindow* window, int key, int scancode, int action, int mods) {
	(gamestate).buttons(key, scancode, action, mods);
}

void mouse(GLFWwindow* window, double x, double y) {
	gamestate.mouse(x, y);
}

int main() {
	char inChar = 'a';
	while (inChar != 'j' && inChar != 'h') {
		std::cout << "Would you like to (j)oin or (h)ost a game: ";
		std::cin >> inChar;
	}

	uint32_t ip = 0;
	std::string ipString;
	if(inChar == 'j') { // client
		loop: while (ip == 0) {
			try {
				std::cout << "Please enter the IP address: ";
				std::cin >> ipString;
				int lastPart = 0, part;
				for (int i = 0; i < 4; i++) {
					int part = ipString.find('.', lastPart + 1);
					ip |= std::stoi(ipString.substr(lastPart + 1, part)) << (8 * (3 - i));
					lastPart = part;
				}
			} catch (std::invalid_argument) {
				ip = 0;
			}
		}
	}

	// std::cout << ip << "\n"; // debug

	/*
	int port = -1;
	while (true) {
		std::cout << "Please enter the port number (0 - 25565): ";
		std::cin >> port;
		if (port > 0 && port <= 25565) {
			break;
		}
	*/

	GameState gamestate = GameState();

	PlayerState serverState = { 0.f, 0.f, 0.f }, clientState = { 0.f, 0.f, 0.f };

	if (inChar == 'j') {
		gamestate.client.SetPlayerState(clientState);
		std::thread clientThread(&Client::Connect, &gamestate.client, ipString);
		puts("Joining game...\n");
		while (!gamestate.client.JoinedServer())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	} else {
		gamestate.server.SetPlayerState(serverState);
		std::thread serverThread(&Server::Serve, &gamestate.server);

		printf("ClientJoined() = %d\n", gamestate.server.ClientJoined());
		puts("Waiting for client to join....\n");
		while (!gamestate.server.ClientJoined()) // lalala too lazy to use mutex idcidc
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		puts("Press enter to start game...\n");
		std::string e;
		std::getline(std::cin, e);

		gamestate.server.startGame(false);
	}

	window = initWindow();
	gamestate.id = gamestate.addPlayer(0);

	//test code
	gamestate.addPlayer(1);
	//not test code

	gamestate.startGame();
	gamestate.setLocalPlayerPosition();
	//glfwSetKeyCallback(window, input);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//if (glfwRawMouseMotionSupported())
	//	glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	glfwSetCursorPosCallback(window, mouse);
	Raycaster raycaster = Raycaster(&gamestate, SIZE_X, SIZE_Y, SCALE, false);
	return renderLoop(window, raycaster);
}