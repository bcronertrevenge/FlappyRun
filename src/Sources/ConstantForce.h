#pragma once

#include <glm/glm.hpp>
#include "Sources/Force.h"

class ConstantForce : public Force
{
public:
	glm::vec3 m_Force;

	ConstantForce(glm::vec3 force);
	~ConstantForce();

	virtual void apply(const std::vector<MovableObject*>& objects);
	virtual void apply(MovableObject* object);

	void SetForce(glm::vec3 _force);
	glm::vec3 GetForce();

};

