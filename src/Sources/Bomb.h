#pragma once
#include <glm/glm.hpp>

class Bomb
{
public:
	Bomb(glm::vec3 _pos);
	~Bomb();

	void Explode();
private:
	glm::vec3 Position;
	glm::vec3 size;
};

