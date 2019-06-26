#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const float SPEED = 3.0f;
const float RUNNING_SPEED = 10.0f;
const float SENSITIVITY = 0.1f;
const float PITCH = 0.0f;
const float YAW = -90.0f;
const float FOV = 50.0f;

class Camera 
{
public:
	// vectors to describe camera in space
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;

	// used to calculate right vector
	glm::vec3 worldUp;

	// properties
	float speed;
	float runningSpeed;
	float sensitivity;
	float pitch;
	float yaw;
	float fov;

	Camera(
		glm::vec3 position = glm::vec3(0.0f, 2.0f, 3.0f),
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f),
		float fov = FOV,
		float speed = SPEED,
		float runningSpeed = RUNNING_SPEED,
		float sensitivity = SENSITIVITY,
		float pitch = PITCH,
		float yaw = YAW);
	glm::mat4 getViewMatrix();
	void processKeyboard(Movement_type direction, float deltaTime, bool jump = false, bool run = false);
	void processMouse(float xoffset, float yoffset, GLboolean constrainPitch = true);
private:
	void updateVectors();
};