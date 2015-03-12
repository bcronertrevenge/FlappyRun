#pragma once

#include <glm/glm.hpp>

class MovableObject
{
public:
	MovableObject(glm::vec3 _pos, float sizeObject);
	~MovableObject();

	virtual glm::vec3 GetPosition();
	virtual void SetPosition(glm::vec3 _pos);

	virtual glm::vec3 GetVelocity();
	virtual void SetVelocity(glm::vec3 _vel);

	virtual float GetMass();
	virtual void SetMass(float _mass);

	virtual glm::vec3 GetForce();
	virtual void SetForce(glm::vec3 _force);

	virtual void AddForce(glm::vec3 _force);
	virtual float GetSize();
	virtual bool CheckHitObject(MovableObject *_object);

protected:
	glm::vec3 m_Position;
	glm::vec3 m_Velocity;
	glm::vec3 m_Force;
	float m_Mass;

	float sizeObject;
};

