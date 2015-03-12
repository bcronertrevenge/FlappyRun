#pragma once

#include <glm/glm.hpp>
#include "Sources/Force.h"

struct ObjectState
{
	glm::vec3 position;
	glm::vec3 velocity;
};

class LeapfrogSolver {
public:
	LeapfrogSolver();
	~LeapfrogSolver();
	void solve(const std::vector<MovableObject*>& objects, float dt);
	void solve(MovableObject* obj, float dt);
	ObjectState getNextState(MovableObject* obj, float dt) const;
};

