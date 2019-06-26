#pragma once

#include <glm/glm.hpp>

#include "player.h"


class PlayerCamera
{
public:
	glm::vec3 position;
	float distanceFromPlayer = 10.0f;
	float angleAroundPlayer = 0.0f;

	float pitch = 20.0f;
	float fov = 45.0f;
	glm::mat4 V;
	Player* player;

	PlayerCamera(Player* player);
	void move();
	glm::mat4 getViewMatrix();
	void calculatePitch(float yoffset);
	void calculateAngleAroundPlayer(float xoffset);
	void calculateZoom(float yoffset);
	void setFov(float fov);
private:
	float calculateHorizontalDistance();
	float calculateVerticalDistance();
	void calculateCameraPosition(float horizontal, float vertical);

};
