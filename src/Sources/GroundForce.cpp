#include "Sources/GroundForce.h"
#include "Sources/MovableObject.h"
#include "Sources/LeapFrogSolver.h"
#include <glm/glm.hpp>

GroundForce::GroundForce(float elasticity, const LeapfrogSolver& solver, float _y)
	: m_fDt(0), m_fElasticity(elasticity), groundY(_y)
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

	glm::vec3 normal(0.f, 1.f, 0.f);
	for (MovableObject* obj : objects)
	{
		ObjectState state = m_Solver->getNextState(obj, m_fDt);

		if (state.position.y - obj->GetSize() <= groundY)
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
	glm::vec3 normal(0.f, 1.f, 0.f);

	if (state.position.y - obj->GetSize() <= groundY)
	{
		obj->AddForce(m_fElasticity * glm::dot(state.velocity, -normal) * (obj->GetMass() / m_fDt) * normal);
	}
}