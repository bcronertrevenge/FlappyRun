#pragma once
#include <glm/glm.hpp>
#include <vector>

class MovableObject;

class Force
{
public:
	Force(){}
	virtual ~Force(){}

	virtual void apply(const std::vector<MovableObject*>& objects) = 0;
	virtual void apply(MovableObject* object) = 0;
};

