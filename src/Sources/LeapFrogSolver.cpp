#include "Sources/LeapfrogSolver.h"
#include "MovableObject.h"
#include <glm/glm.hpp>

LeapfrogSolver::LeapfrogSolver()
{

}

LeapfrogSolver::~LeapfrogSolver()
{

}

void LeapfrogSolver::solve(const std::vector<MovableObject*>& objects, float dt)
{
	for (MovableObject * obj : objects)
	{
		obj->SetVelocity(obj->GetVelocity() + dt * (obj->GetForce() / obj->GetMass()));
		obj->SetPosition(obj->GetPosition() + dt * obj->GetVelocity());

		//Remise de la force à 0
		obj->SetForce(glm::vec3(0.f));
	}
}

void LeapfrogSolver::solve(MovableObject* obj, float dt)
{
	obj->SetVelocity(obj->GetVelocity() + dt * (obj->GetForce() / obj->GetMass()));
	obj->SetPosition(obj->GetPosition() + dt * obj->GetVelocity());

	//Remise de la force à 0
	obj->SetForce(glm::vec3(0.f));
}

ObjectState LeapfrogSolver::getNextState(MovableObject* obj, float dt) const
{
	ObjectState particleState;
	particleState.velocity = obj->GetVelocity() + dt * (obj->GetForce() / obj->GetMass());
	particleState.position = obj->GetPosition() + dt * particleState.velocity;

	return particleState;
}