#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

#include <iostream>
#include <vector>

#include "shader.h"
#include "model.h"
#include "playerCamera.h"
#include "player.h"

// screen / window settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
const unsigned int BOARD_SIZE = 6;
const float HEIGHT_SCALE = 1.8f;
const float WIDTH_SCALE = 20.0f;
const int BUILDING_HEIGHT = 12;
float aspectRatio = (float) SCR_WIDTH / (float) SCR_HEIGHT;

// used to process mouse position
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

int board[BOARD_SIZE][BOARD_SIZE];

// Objects
std::vector<Model> models;
std::vector<Shader> shaders;
Player* player;
glm::vec3 lastPlayerPosition;
PlayerCamera* camera;

// functions
GLFWwindow* initialize();
void initBoard();
void errorCallback(int error, const char* description);
void windowResizeCallback(GLFWwindow* window, int width, int height);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void checkCollisions();
bool pointInsideCube(glm::vec3 point, glm::vec3 oPos, glm::vec3 oCol);
void cursorCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void createModels();
void prepare();
void draw(GLFWwindow* window, glm::mat4 V, glm::mat4 P);
void freeOpenGLProgram(GLFWwindow* window);


int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // flag checking memory leaks
	GLFWwindow* window = initialize();

	initBoard();
	createModels();

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		glm::mat4 V = camera->getViewMatrix();
		glm::mat4 P = glm::perspective(glm::radians(camera->fov), aspectRatio, 0.01f, 1000.0f);

		prepare(); // prepare global variables
		checkCollisions();
		player->move(deltaTime);
		camera->move();
		draw(window, V, P); // draw to back buffer

		// Swap buffers and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clearing all previously allocated resources.
	freeOpenGLProgram(window);

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void initBoard()
{
	srand(136749);
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			board[i][j] = rand() % BUILDING_HEIGHT;
		}
	}
}

GLFWwindow* initialize()
{
	GLFWwindow* window;

	// glfw initialization
	glfwSetErrorCallback(errorCallback);
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW.\n");
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Roof Jumper", NULL, NULL);
	if (!window)
	{
		fprintf(stderr, "Failed to create window.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // v sync

	// glad initialization
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		fprintf(stderr, "Failed to initialize GLAD.\n");
		exit(EXIT_FAILURE);
	}

	// additional settings
	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST); // z buffer
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, cursorCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // hide cursor

	return window;
}

void errorCallback(int error, const char* description) 
{
	fputs(description, stderr);
}

void windowResizeCallback(GLFWwindow* window, int width, int height)
{
	if (height == 0) return;
	aspectRatio = (float)width / (float)height;
	glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, true);
		if (key == GLFW_KEY_W) player->processKeyboard(FORWARD);
		if (key == GLFW_KEY_A) player->processKeyboard(LEFT);
		if (key == GLFW_KEY_S) player->processKeyboard(BACKWARD);
		if (key == GLFW_KEY_D) player->processKeyboard(RIGHT);
		if (key == GLFW_KEY_LEFT_SHIFT && glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			camera->setFov(46.0f);
			player->processKeyboard(RUN);
		}
		if (key == GLFW_KEY_SPACE) player->processKeyboard(JUMP);
		lastPlayerPosition = player->getPosition();
	}
	if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, true);
		if (key == GLFW_KEY_W) player->processKeyboard(STOP_MOVE);
		if (key == GLFW_KEY_A) player->processKeyboard(STOP_ROTATE);
		if (key == GLFW_KEY_S) player->processKeyboard(STOP_MOVE);
		if (key == GLFW_KEY_D) player->processKeyboard(STOP_ROTATE);
		if (key == GLFW_KEY_LEFT_SHIFT && glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			camera->setFov(45.0f);
			player->processKeyboard(STOP_RUN);
		}
		lastPlayerPosition = player->getPosition();
	}

	if (action == GLFW_REPEAT)
	{
		if (key == GLFW_KEY_SPACE) player->processKeyboard(JUMP);
		lastPlayerPosition = player->getPosition();
	}
}

void checkCollisions()
{
	glm::vec3 pPos = player->getPosition();
	int x = (int) round(pPos.x / WIDTH_SCALE) % BOARD_SIZE;
	int z = (int) round(pPos.z / WIDTH_SCALE) % BOARD_SIZE;

	player->setTerrainHeightNext(board[x][z] * HEIGHT_SCALE);
}

// checks if given point is inside cube with bottom center at oPos and dimensions 2x * y * 2z
bool pointInsideCube(glm::vec3 point, glm::vec3 oPos, glm::vec3 oCol)
{
	glm::vec3 actualPosition = player->getPosition();
	// root of model is on the bottom, so dont subtract oCol.y
	if (point.x < oPos.x + oCol.x && point.x > oPos.x - oCol.x
		&& point.y < oPos.y + oCol.y && point.y > oPos.y
		&& point.z < oPos.z + oCol.z && point.z > oPos.z - oCol.z)
	{
		float x1, x2, y, z1, z2;
		x1 = point.x - (oPos.x + oCol.x);
		x2 = point.x - (oPos.x - oCol.x);
		y = point.y - (oPos.y + oCol.y);
		z1 = point.z - (oPos.z + oCol.z);
		z2 = point.z - (oPos.z - oCol.z);
		// TODO move player outside collider

		lastPlayerPosition += actualPosition;
		return true;
	}

	return false;
}

void cursorCallback(GLFWwindow* window, double xpos, double ypos)
{
	// when mouse first entered window location was far from the center, 
	// so set last position as first entered
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		camera->calculateAngleAroundPlayer(xoffset);
		camera->calculatePitch(yoffset);
	}

	player->processMouse(xoffset);
	camera->move();
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera->calculateZoom(yoffset);
}

void createModels()
{
	Shader modelShader("vertexShader.glsl", "fragmentShader.glsl");
	shaders.push_back(modelShader);

	Model building("models/NY_building/13940_New_York_City_Brownstone_Building_v1_l2.obj", glm::vec3(2.0f, 2.0f, 2.0f));
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			models.push_back(building);
		}
	}

	player = new Player("models/nanosuit/nanosuit.obj");
	lastPlayerPosition = player->getPosition();
	int x = (int) round(lastPlayerPosition.x / WIDTH_SCALE) % BOARD_SIZE;
	int z = (int) round(lastPlayerPosition.z / WIDTH_SCALE) % BOARD_SIZE;
	lastPlayerPosition.y = board[x][z]* HEIGHT_SCALE;
	player->setTerrainHeight(board[x][z]* HEIGHT_SCALE);
	player->setTerrainHeightNext(board[x][z]* HEIGHT_SCALE);
	player->respectCollision(lastPlayerPosition);
	
	camera = new PlayerCamera(player);
}

void prepare()
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

void draw(GLFWwindow* window, glm::mat4 V, glm::mat4 P)
{
	glClearColor(0.2f, 0.3f, 0.7f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 M;

	shaders[0].use();
	shaders[0].setMat4("P", P);
	shaders[0].setMat4("V", V);
	
	M = glm::mat4(1.0f);
	shaders[0].setMat4("M", M);
	player->draw(shaders[0]);

	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			if (board[i][j] != 0)
			{
				M = glm::mat4(1.0f);
				M = glm::translate(M, glm::vec3(i * 20, 0.0f, j * 20));
				M = glm::scale(M, glm::vec3(0.03f, board[i][j] / 500.0f, 0.05f));
				M = glm::rotate(M, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				shaders[0].setMat4("M", M);
				models[i + j].draw(shaders[0]);
			}
		}
	}
}

void freeOpenGLProgram(GLFWwindow* window)
{
	delete camera;
	delete player;
}