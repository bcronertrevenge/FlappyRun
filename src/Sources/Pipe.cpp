#include "Pipe.h"


Pipe::Pipe(glm::vec3 pos, float widthmap):
	widthmap(widthmap)
{
	Position = glm::vec3(pos);
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
	if (Position.z < -(widthmap / 2) || Position.z > widthmap / 2)
	{
		return true;
	}
	return false;
}