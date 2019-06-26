#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "player.h"

Player::Player(const char* path)
{
	model = new Model(path, glm::vec3(2.0f, 4.0f, 2.0f));
	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(3.0f, 0.0f, 0.0f));
	M = glm::scale(M, glm::vec3(0.2f, 0.2f, 0.2f));
	rotationAngle = 0.0f;
	inAir = false;

}

Player::~Player()
{
	delete model;
}

void Player::draw(Shader shader)
{
	shader.setMat4("M", M);
	model->draw(shader);
}

void Player::move(float deltaTime)
{
	float tmpHeight = terrainHeight;
	glm::mat4 tmpM = glm::rotate(M, glm::radians(rotationSpeed * deltaTime), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationAngle += rotationSpeed * deltaTime;
	if (rotationAngle > 359.9f)
		rotationAngle -= 359.9f;

	upwardsSpeed += GRAVITY * deltaTime;
	tmpM = glm::translate(tmpM, glm::vec3(0.0f, 0.0f, currentSpeed * deltaTime));
	tmpM = glm::translate(tmpM, glm::vec3(0.0f, upwardsSpeed * deltaTime, 0.0f));
	if ((terrainHeight - tmpM[3].y < 0.5f) && (terrainHeight - tmpM[3].y >= 0.0f)) // TODO To po || pozwala na teleport na zadan¹ wysokosæ
	{
		std::cout << "1: " << terrainHeight - tmpM[3].y << "\n";
		upwardsSpeed = 0.0f;
		inAir = false;
		tmpM[3].y = terrainHeight;
		//terrainHeight = terrainHeightNext;
	}
	else if (terrainHeight < tmpM[3].y)
	{

	}
	else if (terrainHeightNext > tmpM[3].y + 0.5f)
	{
		std::cout << "2: " << tmpM[3].y << "terr:" << terrainHeightNext << "tmp: "  << tmpM[3].y << "WYR: " << (terrainHeight - tmpM[3].y < 0.5f) << (terrainHeight - tmpM[3].y >= 0.0f) << "\n";

		upwardsSpeed = 0.0f;
		inAir = false;
		tmpM = M;
		terrainHeightNext = terrainHeight;
	}

	if (terrainHeightNext < tmpM[3].y)
	{
		std::cout << "3\n";
		terrainHeight = terrainHeightNext;
	}

	std::cout << "terrainHeight: " << terrainHeight << " | terrainHeightNext: " << terrainHeightNext << " | y: " << tmpM[3].y << "\n";
	M = tmpM;
}

void Player::respectCollision(glm::vec3 newPosition)
{
	M[3].x = newPosition.x;
	M[3].y = newPosition.y;
	M[3].z = newPosition.z;
}

void Player::processKeyboard(Movement_type type)
{
	if (type == FORWARD)
		currentSpeed = MOVE_SPEED;
	if (type == RUN)
		currentSpeed = RUN_SPEED;
	if (type == JUMP)
		if (!inAir)
		{
			upwardsSpeed = JUMP_POWER;
			inAir = true;
		}
			
	if (type == BACKWARD)
		currentSpeed = -MOVE_SPEED;
	if (type == LEFT)
		rotationSpeed = TURN_SPEED;
	if (type == RIGHT)
		rotationSpeed = -TURN_SPEED;
	if (type == STOP_MOVE)
		currentSpeed = 0.0f;
	if (type == STOP_RUN)
		currentSpeed = MOVE_SPEED;
	if (type == STOP_ROTATE)
		rotationSpeed = 0.0f;
}

void Player::processMouse(float xoffset)
{
	float angle = -xoffset * 0.1f;
	M = glm::rotate(M, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationAngle += angle;
	if (rotationAngle > 359.9f)
		rotationAngle -= 359.9f;
}

glm::vec3 Player::getPosition()
{
	return glm::vec3(M[3]);
}

glm::vec3 Player::getCollider()
{
	return model->collider;
}

void Player::setTerrainHeight(float height)
{
	terrainHeight = height;
}

void Player::setTerrainHeightNext(float height)
{
	terrainHeightNext = height;
}