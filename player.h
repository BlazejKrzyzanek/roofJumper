#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "model.h"

enum Movement_type {
	FORWARD,
	BACKWARD,
	RIGHT,
	LEFT,
	STOP_MOVE,
	STOP_ROTATE,
	STOP_RUN,
	JUMP,
	RUN
};

const float MOVE_SPEED = 50.0f;
const float RUN_SPEED = 90.0f;
const float TURN_SPEED = 90.0f; // degrees per second
const float GRAVITY = -80.0f;
const float JUMP_POWER = 90.0f;

class Player
{
public:
	glm::mat4 M;
	float currentSpeed;
	float rotationSpeed;
	float upwardsSpeed;
	bool inAir;
	float terrainHeightNext = 0.0f;
	float terrainHeight = 0.0f;

	float rotationAngle;


	Player(const char* path);
	~Player();
	void draw(Shader shader);
	void move(float deltaTime);
	void processKeyboard(Movement_type type);
	void processMouse(float xoffset);
	void respectCollision(glm::vec3 newPosition);
	glm::vec3 getPosition();
	glm::vec3 getCollider();
	void setTerrainHeight(float height);
	void setTerrainHeightNext(float height);
private:
	Model* model;
};
