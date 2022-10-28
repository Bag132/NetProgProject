#include "Topdown.h"

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0 in vec3 aPos;\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
"}\n\0";

Topdown::Topdown(GameState *_state, int _sizx, int _sizy, int _scale, bool rgba) {
	sizx = _sizx;
	sizy = _sizy;
	scale = _scale;
	pbuff = PixelBuffer(sizx*scale, sizy*scale, rgba ? 4 : 3);
	state = _state;

	/*
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	*/
}

void Topdown::drawTopdown(Vector2 pos) {
	pbuff.clear();
	pbuff.operate([&](int x, int y) {
		return 0xffffffff * map[(((y / scale) * sizx) + (x / scale))];
	});
	pbuff.operate([](int x, int y) {return 0xbb22eeff;}, (*state).pp * scale, ((*state).pp + 1) * scale);
	pbuff.drawBuffer(pos);
}