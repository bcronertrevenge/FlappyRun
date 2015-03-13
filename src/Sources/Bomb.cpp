#include "Bomb.h"
#include "Player.h"
#include "Bird.h"

Bomb::Bomb(glm::vec3 _pos, float endZ)
	: MovableObject(_pos, 1), endZ(endZ), active(false), picked(false), readyToExplode(false)
{
}


Bomb::~Bomb()
{
}

void Bomb::ExplodeBird(Bird* bird)
{
	active = false;
	m_Position.x = rand() % 11 - 5.f;
	m_Position.z = -80.f;
	readyToExplode = false;
}

void Bomb::Move(float speed)
{
	if (active && picked == false)
	{
		m_Position.z += speed;
	}	
}

void Bomb::SetPosition(glm::vec3 _pos)
{
	m_Position = _pos;
}

void Bomb::SetActive(bool _active)
{
	active = _active;
}

bool Bomb::IsActive()
{
	return active;
}

bool Bomb::IsPicked()
{
	return picked;
}

bool Bomb::isOutOfMap()
{
	if (m_Position.z > endZ)
	{
		return true;
	}
	return false;
}

void Bomb::SetPicked(bool _picked)
{
	picked = _picked;
}

void Bomb::SetReadyToExplode(bool _explode)
{
	readyToExplode = _explode;
}

bool Bomb::IsReadyToExplode()
{
	return readyToExplode;
}