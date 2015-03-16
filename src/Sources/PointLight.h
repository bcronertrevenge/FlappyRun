#pragma once
#include <glm/glm.hpp>
#include "Bird.h"

class PointLight : public MovableObject
{
public:
	PointLight(glm::vec3 pos, glm::vec3 color, float intensity);
	~PointLight();

	glm::vec4 getPosition();

private:
	glm::vec3 color;
	float intensity;

	bool hit;
	bool hasAlreadyPassPlayer;
};

