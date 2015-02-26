#pragma once

#include <glm/glm.hpp>
#include "Player.h"

class Bird
{
public:
	Bird(Player* _player);
	~Bird();

	void KillBird();

private:
	glm::vec3 Position;
	glm::vec3 size;
	float speed;
	Player* player;
};

