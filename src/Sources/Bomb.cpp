#include "Bomb.h"
#include "Player.h"
#include "Bird.h"

Bomb::Bomb(glm::vec3 _pos, float endZ)
	: endZ(endZ), Position(_pos), active(false), picked(false), readyToExplode(false)
{
	sizeBomb = 1;
}


Bomb::~Bomb()
{
}

void Bomb::ExplodeBird(Bird* bird)
{
	active = false;
	Position.x = rand() % 15 - 10.f;
	Position.z = -80.f;
	readyToExplode = false;
	bird->KillBird();
}

void Bomb::Move(float speed)
{
	if (active && picked == false)
	{
		Position.z += speed;
	}	
}

glm::vec3 Bomb::GetPosition()
{
	return Position;
}

void Bomb::SetPosition(glm::vec3 _pos)
{
	Position = _pos;
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

bool Bomb::CheckHitPlayer(Player *_player)
{
	if (glm::distance(_player->GetPosition(), Position) < (sizeBomb + _player->GetSize()) / 2)
	{
		return true;
	}
	return false;
}

bool Bomb::CheckHitBird(Bird *_bird)
{
	if (glm::distance(_bird->GetPosition(), Position) < (sizeBomb + _bird->GetSize()) / 2 && active && readyToExplode)
	{
		return true;
	}
	return false;
}

bool Bomb::isOutOfMap()
{
	if (Position.z > endZ)
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