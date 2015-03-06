#include "Bird.h"
#include <iostream>

Bird::Bird(Player* _player) : player(_player), isBirdDragged(false)
{
	Position = glm::vec3(player->GetPosition().x, player->GetPosition().y, player->GetPosition().z + 10.f);
	sizeBird = 1;
	speed = 0.0025f;
}


Bird::~Bird()
{
}

void Bird::Move()
{
	if (isBirdDragged) return;

	if (Position.z > player->GetPosition().z && player->GetSpeed() < 0.25f)
	{
		Position.z -= speed;
	}
	else if (Position.z < 10.f && player->GetSpeed() > 0.75f)
	{
		Position.z += speed;
	}

	if (Position.x < player->GetPosition().x)
	{
		Position.x += speed;
	}
	else if (Position.x > player->GetPosition().x)
	{
		Position.x -= speed;
	}

	if (glm::abs(Position.x - player->GetPosition().x) < speed)
	{
		Position.x = player->GetPosition().x;
	}
}

void Bird::KillBird()
{
	//TODO
	std::cout << "Boom" << std::endl;
}

glm::vec3 Bird::GetPosition()
{
	return Position;
}

float Bird::GetSize()
{
	return sizeBird;
}

bool Bird::CheckHitPlayer()
{
	if (glm::distance(player->GetPosition(), Position) < (sizeBird + player->GetSize()) / 2)
	{
		return true;
	}
	return false;
}

void Bird::SetDragged(bool _drag)
{
	isBirdDragged = _drag;
}

void Bird::SetPosZ(float z)
{
	Position.z = z;
}