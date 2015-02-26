#include "Player.h"


Player::Player() : hasBomb(false), isDead(false)
{
	Position = glm::vec3(0);
	size = glm::vec3(2,2,3);
	speed = 2;
}


Player::~Player()
{
}

void Player::DropBomb()
{
	if (hasBomb)
	{
		//TODO
	}
}

void Player::KillPlayer()
{
	//TODO
}

glm::vec3 Player::GetPosition()
{
	return Position;
}