#include "Sources/Player.h"


Player::Player(float width) : hasBomb(false), isDead(false), width(width)
{
	Position = glm::vec3(0);
	size = glm::vec3(2,2,3);
	speed = 0.1f;
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

void Player::MoveLeft()
{
	if (-(width/2) < Position.x)
	{
		Position.x -= speed;
	}	
}

void Player::MoveRight()
{
	if (width / 2 > Position.x)
	{
		Position.x += speed;
	}
}
