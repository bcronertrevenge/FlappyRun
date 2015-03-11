#include "Sources/GroundForce.h"
#include "Sources/MovableObject.h"
#include "Sources/LeapFrogSolver.h"
#include <iostream>
#include <glm/glm.hpp>

GroundForce::GroundForce(float elasticity, const LeapfrogSolver& solver, float _y, bool floor, glm::vec3 _normal, bool sup)
	: m_fDt(0), m_fElasticity(elasticity), groundYZ(_y), ground(floor), normal(_normal), superior(sup)
{
	m_Solver = &solver;
}

GroundForce::~GroundForce(){}


void GroundForce::setDt(float dt)
{
	m_fDt = dt;
}

void GroundForce::apply(const std::vector<MovableObject*>& objects)
{
	if (m_fDt <= 0.f) return;

	for (MovableObject* obj : objects)
	{
		ObjectState state = m_Solver->getNextState(obj, m_fDt);

		if ((state.position.y - obj->GetSize() / 2 < groundYZ && ground && !superior) || (state.position.z - obj->GetSize() / 2 < groundYZ && !ground && !superior) || (state.position.y + obj->GetSize() / 2 > groundYZ && ground && superior) || (state.position.z + obj->GetSize() / 2 > groundYZ && !ground && superior))
		{
			obj->AddForce(m_fElasticity * glm::dot(state.velocity, -normal) * (obj->GetMass() / m_fDt) * normal);
			obj->GetPosition();
		}
	}
}

void GroundForce::apply(MovableObject* obj)
{
	if (m_fDt <= 0.f) return;

	ObjectState state = m_Solver->getNextState(obj, m_fDt);

	if ((state.position.y - obj->GetSize() / 2 < groundYZ && ground && !superior) || (state.position.z - obj->GetSize() / 2 < groundYZ && !ground && !superior) || (state.position.y + obj->GetSize() / 2 > groundYZ && ground && superior) || (state.position.z + obj->GetSize() / 2 > groundYZ && !ground && superior))
	{
		obj->AddForce(m_fElasticity * glm::dot(state.velocity, -normal) * (obj->GetMass() / m_fDt) * normal);
	}
}