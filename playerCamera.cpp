#include <glm/glm.hpp>

#include "playerCamera.h"

PlayerCamera::PlayerCamera(Player* player)
{
	this->player = player;
	V = glm::mat4(1.0f);
	move();
}

glm::mat4 PlayerCamera::getViewMatrix()
{
	return glm::lookAt(position, player->getPosition() + glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void PlayerCamera::move()
{
	float horizontal = calculateHorizontalDistance();
	float vertical = calculateVerticalDistance();
	calculateCameraPosition(horizontal, vertical);
}

void PlayerCamera::calculatePitch(float yoffset)
{
	float pitchChange = yoffset * 0.1f;
	pitch -= pitchChange;
	if (pitch < 8.0f)
		pitch = 8.0f;
	else if (pitch > 70.0f)
		pitch = 70.0f;
}

void PlayerCamera::calculateAngleAroundPlayer(float xoffset)
{
	float angleChange = xoffset * 0.3f;
	angleAroundPlayer -= angleChange;
}

void PlayerCamera::calculateZoom(float yoffset)
{
	distanceFromPlayer -= yoffset;
	if (distanceFromPlayer < 3.0f)
		distanceFromPlayer = 3.0f;
	else if (distanceFromPlayer > 50.0f)
		distanceFromPlayer = 50.0f;
}

float PlayerCamera::calculateHorizontalDistance()
{
	return distanceFromPlayer * glm::cos(glm::radians(pitch));
}

float PlayerCamera::calculateVerticalDistance()
{
	return distanceFromPlayer * glm::sin(glm::radians(pitch));
}

void PlayerCamera::calculateCameraPosition(float horizontal, float vertical)
{
	float angle = player->rotationAngle + angleAroundPlayer;
	float xoffset = horizontal * glm::sin(glm::radians(angle));
	float zoffset = horizontal * glm::cos(glm::radians(angle));
	position.x = player->getPosition().x - xoffset;
	position.y = player->getPosition().y + vertical;
	position.z = player->getPosition().z - zoffset;
}

void PlayerCamera::setFov(float angle)
{
	fov = angle;
}

