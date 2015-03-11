#include "Sources/Player.h"
#include <iostream>

Player::Player(float width) : MovableObject(glm::vec3(0), 1), hasBomb(false), isDead(false), width(width), BombPicked(NULL)
{
	jumping = false;
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

bool Player::IsDead()
{
	return isDead;
}

bool Player::HasBomb()
{
	return BombPicked != NULL;
}

void Player::Jump(float _time)
{
	if (jumping || m_Position.y >= 1.f || _time - LastTimeJump < 3.f) // A changer pour sauter moins souvent
		return;

	jumping = true;
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

void Player::SetJumping(bool _jump)
{
	jumping = _jump;
}