#include "Sources/Player.h"
#include <iostream>

Player::Player(float width) : hasBomb(false), isDead(false), width(width), BombPicked(NULL)
{
	Position = glm::vec3(0);
	sizePlayer = 1;
	speed = 0.25f;
	speedMax = 0.75f;
	speedMin = 0.1f;
}


Player::~Player()
{
}

void Player::PickBomb(Bomb* bomb)
{
	BombPicked = bomb;
	bomb->SetActive(false);
	bomb->SetPicked(true);

	std::cout << "Bomb Picked" << std::endl;
}

void Player::DropBomb()
{
	if (BombPicked != NULL)
	{
		BombPicked->SetPosition(glm::vec3(Position.x, Position.y, Position.z + 1.f));
		BombPicked->SetActive(true);
		BombPicked->SetPicked(false);
		BombPicked->SetReadyToExplode(true);
		BombPicked = NULL;

		std::cout << "Bomb Dropped" << std::endl;
	}
}

void Player::KillPlayer()
{
	isDead = true;
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
	speed = glm::min(speed + speedMax / 20.f, speedMax);
}

void Player::SlowDown()
{
	speed = glm::max(speed - speedMax / 5.f, speedMin);
}

bool Player::IsDead()
{
	return isDead;
}

bool Player::HasBomb()
{
	return BombPicked != NULL;
}

float Player::GetMaxSpeed()
{
	return speedMax;
}