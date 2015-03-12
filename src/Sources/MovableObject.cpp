#include "Sources/MovableObject.h"
#include <glm/glm.hpp>

MovableObject::MovableObject(glm::vec3 _pos, float sizeObject) : 
m_Position(_pos),
sizeObject(sizeObject),
m_Force(glm::vec3(0.f)),
m_Velocity(glm::vec3(0.f)),
m_Mass(1.f)
{
}


MovableObject::~MovableObject()
{
}

glm::vec3 MovableObject::GetPosition()
{
	return m_Position;
}

void MovableObject::SetPosition(glm::vec3 _pos)
{
	m_Position = _pos;
}

glm::vec3 MovableObject::GetVelocity()
{
	return m_Velocity;
}

void MovableObject::SetVelocity(glm::vec3 _vel)
{
	m_Velocity = _vel;
}

float MovableObject::GetMass()
{
	return m_Mass;
}

void MovableObject::SetMass(float _mass)
{
	m_Mass = _mass;
}

glm::vec3 MovableObject::GetForce()
{
	return m_Force;
}

void MovableObject::SetForce(glm::vec3 _force)
{
	m_Force = _force;
}

void MovableObject::AddForce(glm::vec3 _force)
{
	m_Force += _force;
}

float MovableObject::GetSize()
{
	return sizeObject;
}

bool MovableObject::CheckHitObject(MovableObject *_object)
{
	if (glm::distance(_object->GetPosition(), m_Position) < (sizeObject + _object->GetSize()) / 2)
	{
		return true;
	}
	return false;
}