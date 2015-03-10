#include "Sources/Player.h"
#include <iostream>

Player::Player(float width) : MovableObject(glm::vec3(0), 1), hasBomb(false), isDead(false), width(width), BombPicked(NULL)
{
	speed = 0.25f;
	speedMax = 0.75f;
	speedMin = 0.1f;

	heightMax = 2.f;
	ascending = true;
	jumping = false;
	speedJump = 0.2f;
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
		BombPicked->SetPosition(glm::vec3(m_Position.x, m_Position.y, m_Position.z + 1.f));
		BombPicked->SetActive(true);
		BombPicked->SetPicked(false);
		BombPicked->SetReadyToExplode(true);
		BombPicked = NULL;
	}
}

void Player::KillPlayer()
{
	isDead = true;
}

float Player::GetSpeed()
{
	return speed;
}

void Player::MoveLeft()
{
	if (-(width / 2) < m_Position.x)
	{
		m_Position.x -= 0.1f;
	}	
}

void Player::MoveRight()
{
	if (width / 2 > m_Position.x)
	{
		m_Position.x += 0.1f;
	}
}

void Player::Move()
{
	if (jumping && m_Position.y >= heightMax)
	{
		jumping = false;
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

void Player::Jump(float _time)
{
	if (jumping || _time - LastTimeJump < 1.f) // A changer
		return;

	jumping = true;
	ascending = true;
	LastTimeJump = _time;
}

void Player::applyForce(glm::vec3 _pos)
{
	m_Position += _pos;

	if (m_Position.y < 0.f) m_Position.y = 0.f;
}

bool Player::IsJumping()
{
	return jumping;
}