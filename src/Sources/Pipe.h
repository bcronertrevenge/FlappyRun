#pragma once
#include <glm/glm.hpp>

class Pipe
{
public:
	Pipe(glm::vec3 pos, float startZ, float endZ);
	~Pipe();
	void Move(float speed);
	glm::vec3 GetPosition();
	bool isOutOfMap();

private:
	glm::vec3 Position;
	glm::vec3 size;
	float startZ;
	float endZ;

};

