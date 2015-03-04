#include "Pipe.h"


Pipe::Pipe(glm::vec3 pos, float endZ):
endZ(endZ), hit(false), hasAlreadyPassPlayer(false), birdToDrag(NULL)
{
	Position = glm::vec3(pos);
	sizePipe = 1;
}


Pipe::~Pipe()
{
}

void Pipe::Move(float speed)
{
	Position.z += speed;

	if (birdToDrag != NULL)
	{
		birdToDrag->SetPosZ(Position.z + sizePipe / 2);
	}
}

bool Pipe::CheckHitPlayer(Player *_player)
{
	if (glm::distance(_player->GetPosition(), Position) < (sizePipe + _player->GetSize())/2)
	{
		hit = true;
		return true;
	}
	return false;
}

bool Pipe::CheckHitBird(Bird *_bird)
{
	if (glm::distance(_bird->GetPosition(), Position) < (sizePipe + _bird->GetSize()) / 2)
	{
		return true;
	}
	return false;
}

glm::vec3 Pipe::GetPosition()
{
	return Position;
}

void Pipe::SetPosition(glm::vec3 pos)
{
	Position = pos;
}


bool Pipe::isOutOfMap()
{
	if (Position.z > endZ)
	{
		hit = false;
		hasAlreadyPassPlayer = false;

		if (birdToDrag != NULL)
		{
			birdToDrag->SetDragged(false);
			birdToDrag = NULL;
		}
		return true;
	}
	return false;
}

bool Pipe::HasPassedPlayer(Player * _player)
{
	if (hasAlreadyPassPlayer == false && Position.z > _player->GetPosition().z)
	{
		hasAlreadyPassPlayer = true;
		return true;
	}
	return false;
	
}

bool Pipe::hasHit()
{
	return hit;
}

void Pipe::SetDraggedBird(Bird* _bird)
{
	if (birdToDrag == NULL)
		birdToDrag = _bird;
}