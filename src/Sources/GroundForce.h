#pragma once

#include <glm/glm.hpp>
#include "Sources/Force.h"

class LeapfrogSolver;

class GroundForce : public Force
{
public:
	glm::vec3 m_Force;

	GroundForce(float elasticity, const LeapfrogSolver& solver, float _yz, bool floor, glm::vec3 _normal, bool sup);
	~GroundForce();

	void setDt(float dt);

	virtual void apply(const std::vector<MovableObject*>& objects);
	virtual void apply(MovableObject* object);

	float m_fElasticity;
	const LeapfrogSolver* m_Solver;
	float m_fDt;
	float groundYZ;
	bool ground;
	glm::vec3 normal;
	bool superior;
};

