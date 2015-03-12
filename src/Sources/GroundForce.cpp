#include "Sources/GroundForce.h"
#include "Sources/MovableObject.h"
#include "Sources/LeapFrogSolver.h"
#include <iostream>
#include <glm/glm.hpp>

GroundForce::GroundForce(float elasticity, const LeapfrogSolver& solver, float _y, WallType _type, glm::vec3 _normal, bool sup)
	: m_fDt(0), m_fElasticity(elasticity), groundXYZ(_y), wallType(_type), normal(_normal), superior(sup)
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

		float xyz = 0.f;

		switch (wallType)
		{
		case WallX:
			xyz = state.position.x;
			break;
		case Ground:
			xyz = state.position.y;
			break;
		case WallZ:
			xyz = state.position.z;
			break;
		default:
			break;
		}

		if ((xyz - obj->GetSize() / 2 < groundXYZ && !superior) || (xyz + obj->GetSize() / 2 > groundXYZ && superior))
		{
			obj->AddForce(m_fElasticity * glm::dot(state.velocity, -normal) * (obj->GetMass() / m_fDt) * normal);
		}
	}
}

void GroundForce::apply(MovableObject* obj)
{
	if (m_fDt <= 0.f) return;

	ObjectState state = m_Solver->getNextState(obj, m_fDt);

	float xyz = 0.f;

	switch (wallType)
	{
	case WallX:
		xyz = state.position.x;
		break;
	case Ground:
		xyz = state.position.y;
		break;
	case WallZ:
		xyz = state.position.z;
		break;
	default:
		break;
	}

	if ((xyz - obj->GetSize() / 2 < groundXYZ && !superior) || (xyz + obj->GetSize() / 2 > groundXYZ && superior))
	{
		obj->AddForce(m_fElasticity * glm::dot(state.velocity, -normal) * (obj->GetMass() / m_fDt) * normal);
	}
}