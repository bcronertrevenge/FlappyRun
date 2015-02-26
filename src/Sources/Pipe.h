#pragma once
#include <glm/glm.hpp>

class Pipe
{
public:
	Pipe(glm::vec3 pos);
	~Pipe();

private:
	glm::vec3 Position;
	glm::vec3 size;
};

