#include "Bird.h"
#include <iostream>

Bird::Bird(Player* _player) : player(_player), isBirdDragged(false)
{
	Position = glm::vec3(player->GetPosition().x, player->GetPosition().y, player->GetPosition().z + 10.f);
	sizeBird = 1;
	speedIncrement = 0.02f;
}


Bird::~Bird()
{
}

void Bird::Move()
{
	if (isBirdDragged) return;

	if (Position.z > player->GetPosition().z && player->GetSpeed() <= player->GetMaxSpeed() / 2)
	{
		//Position.z -= speedIncrement;
		Position.z += (player->GetSpeed() - player->GetMaxSpeed() / 2) * speedIncrement;
	}
	else if (Position.z < 10.f && player->GetSpeed() > player->GetMaxSpeed() / 2)
	{
		//Position.z += speedIncrement;
		Position.z += (player->GetSpeed() - player->GetMaxSpeed() / 2) * speedIncrement;
	}

	if (Position.x < player->GetPosition().x)
	{
		Position.x += speedIncrement;
	}
	else if (Position.x > player->GetPosition().x)
	{
		Position.x -= speedIncrement;
	}

	if (glm::abs(Position.x - player->GetPosition().x) < speedIncrement)
	{
		Position.x = player->GetPosition().x;
	}
}

void Bird::StepBack(float _step)
{
	Position.z = glm::min(Position.z + _step, player->GetPosition().z + 10.f);
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