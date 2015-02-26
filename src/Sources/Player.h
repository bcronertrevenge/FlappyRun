#pragma once

#include <glm/glm.hpp>

class Player
{
public:
	Player();
	~Player();

	void DropBomb();
	void KillPlayer();

	glm::vec3 GetPosition();

private:
	glm::vec3 Position;
	glm::vec3 size;
	bool hasBomb;
	float speed;
	bool isDead;
};

