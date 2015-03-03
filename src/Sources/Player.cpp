#include "Sources/Player.h"


Player::Player(float width) : hasBomb(false), isDead(false), width(width)
{
	Position = glm::vec3(0);
	sizePlayer = 1;
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

float Player::GetSize()
{
	return sizePlayer;
}

float Player::GetSpeed()
{
	return speed;
}

void Player::MoveLeft()
{
	if (-(width/2) < Position.x)
	{
		Position.x -= 0.1f;
	}	
}

void Player::MoveRight()
{
	if (width / 2 > Position.x)
	{
		Position.x += 0.1f;
	}
}

void Player::SpeedUp()
{
	speed = glm::min(speed + 0.03f, 1.f);
}

void Player::SlowDown()
{
	speed = glm::max(speed - 0.2f, 0.1f);
}