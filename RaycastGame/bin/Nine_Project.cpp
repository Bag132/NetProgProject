// Nine_Project.cpp : Defines the entry point for the application.
//

#include "Nine_Project.h"
#include "Comms.h"
#include <thread>


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
	Server server((u_short) 777);
	std::thread acceptThread(&Server::AcceptClient, &server);

	while (!server.IsListening());

	printf("Creating client\n");

	Client client = Client::getInstance();
	client.Connect("127.0.0.1", (u_short)777);
	client.Send("Rich fart");
	std::cout << "> ";
	std::string input;
	std::getline(std::cin, input);
	exit(0);

	window = initWindow();
	gamestate = GameState(Vector2(2, 2));
	//glfwSetKeyCallback(window, input);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//if (glfwRawMouseMotionSupported())
	//	glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	glfwSetCursorPosCallback(window, mouse);
	Raycaster raycaster = Raycaster(&gamestate, SIZE_X, SIZE_Y, SCALE, true);
	return renderLoop(window, raycaster);
}