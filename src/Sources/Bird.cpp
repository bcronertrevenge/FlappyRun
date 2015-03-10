#include "Bird.h"
#include <iostream>

Bird::Bird(Player* _player, float posX) : 
	distanceMax(7.f),
	MovableObject(glm::vec3(0.f), 1),
	player(_player),
	LastFlap(0.f)
{
	m_Position = glm::vec3(posX, player->GetPosition().y, player->GetPosition().z + distanceMax);
	speedIncrement = 0.02f;
}


Bird::~Bird()
{
}

void Bird::Move(const std::vector<Bird*>& birds)
{
	float oldPosX = m_Position.x;
	float oldPosZ = m_Position.z;

	// Z
	if (m_Position.z > player->GetPosition().z || m_Position.z < distanceMax)
	{
		m_Position.z += (player->GetSpeed() - player->GetMaxSpeed() / 2) * speedIncrement;
	}
	
	for (Bird *bird : birds)
	{
		if (bird != this && CheckHitObject(bird))
		{
			m_Position.z = oldPosZ;
			break;
		}
	}

	// X
	if (m_Position.x < player->GetPosition().x)
	{
		m_Position.x += speedIncrement;
	}
	else if (m_Position.x > player->GetPosition().x)
	{
		m_Position.x -= speedIncrement;
	}

	if (glm::abs(m_Position.x - player->GetPosition().x) < speedIncrement)
	{
		m_Position.x = player->GetPosition().x;
	}

	for (Bird *bird : birds)
	{
		if (bird != this && CheckHitObject(bird))
		{
			m_Position.x = oldPosX;
			break;
		}
	}
}

void Bird::StepBack(float _step, const std::vector<Bird*>& birds)
{
	m_Position.z = glm::min(m_Position.z + _step, player->GetPosition().z + distanceMax);
	
	for (Bird * bird : birds)
	{
		if (bird != this && CheckHitObject(bird))
		{
			// Decalement en X
			if (m_Position.z >= player->GetPosition().z + distanceMax)
			{
				bool collision = true;

				while (collision == true)
				{
					collision = false;
					m_Position.x += _step;

					for (Bird * bird : birds)
					{
						if (bird != this && CheckHitObject(bird))
						{
							collision = true;
						}
					}
				}	
			}
			else
			{
				//Decalement en Z
				StepBack(_step, birds);
			}
			
			break;
		}
	}
}

void Bird::SetPosZ(float z)
{
	m_Position.z = z;
}

void Bird::applyForce(glm::vec3 _pos)
{
	m_Position += _pos;

	if (m_Position.y < 0.f) m_Position.y = 0.f;
}

bool Bird::HasToFlap(float dt)
{
	if (player->GetPosition().y > m_Position.y && (dt - LastFlap > 0.5f))
	{
		LastFlap = dt;
		return true;
	}
	else if (dt - LastFlap > 1.f)
	{
		LastFlap = dt;
		return true;
	}

	return false;
}