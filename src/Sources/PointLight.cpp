#include "PointLight.h"

PointLight::PointLight(glm::vec3 _pos, glm::vec3 _color, float _intensity) :
MovableObject(_pos, 1),
color(_color),
intensity(_intensity)
{}

PointLight::~PointLight()
{}

glm::vec4 PointLight::getPosition()
{
	return glm::vec4(m_Position, 0.0);
}