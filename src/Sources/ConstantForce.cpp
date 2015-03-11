#include "Sources/ConstantForce.h"
#include "Sources/MovableObject.h"
#include <glm/glm.hpp>

ConstantForce::ConstantForce(glm::vec3 force) :m_Force(force){}

ConstantForce::~ConstantForce(){}

void ConstantForce::apply(const std::vector<MovableObject*>& objects)
{
	for (MovableObject *obj : objects)
	{
		obj->AddForce(m_Force);
	}
}

void ConstantForce::apply(MovableObject* object){
	object->AddForce(m_Force);
}

void ConstantForce::SetForce(glm::vec3 _force)
{
	m_Force = _force;
}

glm::vec3 ConstantForce::GetForce()
{
	return m_Force;
}