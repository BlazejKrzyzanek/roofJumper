#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, glm::vec3 front, glm::vec3 worldUp, float fov, float speed, float runningSpeed, float sensitivity, float pitch, float yaw)
{
	this->position = position;
	this->up = up;
	this->front = front;
	this->worldUp = worldUp;
	this->fov = fov;
	this->speed = speed;
	this->runningSpeed = runningSpeed;
	this->sensitivity = sensitivity;
	this->pitch = pitch;
	this->yaw = yaw;

	updateVectors();
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}

void Camera::processKeyboard(Movement_type direction, float deltaTime, bool jump, bool run)
{
	float vel;
	if (run)
		vel = runningSpeed * deltaTime;
	else
		vel = speed * deltaTime;
	if (direction == FORWARD)
		position += front * vel;
	if (direction == BACKWARD)
		position -= front * vel;
	if (direction == RIGHT)
		position += right * vel;
	if (direction == LEFT)
		position -= right * vel;
	position.y = 2.0f;
}

void Camera::processMouse(float xoffset, float yoffset, GLboolean constrainPitch)
{
	yaw += (xoffset * sensitivity);
	pitch += (yoffset * sensitivity);
	
	if (constrainPitch) {
		if (pitch > 89.0f) // looking too much up
			pitch = 89.0f;
		if (pitch < -89.0f) // looking too much down
			pitch = -89.0f;
	}

	updateVectors();
}

void Camera::updateVectors()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}
