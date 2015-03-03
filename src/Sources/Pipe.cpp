#include "Pipe.h"


Pipe::Pipe(glm::vec3 pos, float startZ, float endZ):
startZ(startZ), endZ(endZ)
{
	Position = glm::vec3(pos);
	Position.z += startZ;
	size = glm::vec3(2, 2, 4);
}


Pipe::~Pipe()
{
}

void Pipe::Move(float speed)
{
	Position.z += speed;
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