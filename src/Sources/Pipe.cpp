#include "Pipe.h"


Pipe::Pipe(glm::vec3 pos, float startZ, float endZ):
startZ(startZ), endZ(endZ)
{
	Position = glm::vec3(pos);
	Position.z += startZ;
	sizePipe = 1;
}


Pipe::~Pipe()
{
}

void Pipe::Move(float speed)
{
	Position.z += speed;
}

bool Pipe::CheckHitPlayer(Player *_player)
{
	if (glm::distance(_player->GetPosition(), Position) < (sizePipe + _player->GetSize())/2)
	{
		return true;
	}
	return false;
}

glm::vec3 Pipe::GetPosition()
{
	return Position;
}

bool Pipe::isOutOfMap()
{
	if (Position.z > endZ)
	{
		return true;
	}
	return false;
}