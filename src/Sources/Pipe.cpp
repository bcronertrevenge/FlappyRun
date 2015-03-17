#include "Pipe.h"


Pipe::Pipe(glm::vec3 pos, float endZ):
MovableObject( pos, 1),
endZ(endZ), 
hit(false),
hasAlreadyPassPlayer(false)
{
	loadOBJ("obj/Pipe.obj");
}


Pipe::~Pipe()
{
}

bool Pipe::CheckHitObject(MovableObject *_object)
{
	if (MovableObject::CheckHitObject(_object))
	{
		hit = true;
		return true;
	}

	return false;
}

bool Pipe::isOutOfMap()
{
	if (m_Position.z > endZ)
	{
		hit = false;
		hasAlreadyPassPlayer = false;

		return true;
	}
	return false;
}

bool Pipe::HasPassedPlayer(Player * _player)
{
	if (hasAlreadyPassPlayer == false && m_Position.z > _player->GetPosition().z)
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
