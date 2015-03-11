#include "Bird.h"
#include "ConstantForce.h"
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

void Bird::Move(const std::vector<Bird*>& birds, float dt)
{
	float oldPosX = m_Position.x;

	if (m_Position.x < player->GetPosition().x)
	{
		ConstantForce PushX(glm::vec3(0.75f, 0.f, 0.f));
		PushX.apply(this);
	}
	else if (m_Position.x > player->GetPosition().x)
	{
		ConstantForce PushX(glm::vec3(-0.75f, 0.f, 0.f));
		PushX.apply(this);
	}

	for (Bird *bird : birds)
	{
		if (bird != this && CheckHitObject(bird))
		{
			if (oldPosX > m_Position.x)
			{
				glm::vec3 normal(-1.f, 0.f, 0.f);
				bird->AddForce(2.f * glm::dot(bird->GetVelocity(), -normal) * (bird->GetMass() / dt) * normal);
			}
			else
			{
				glm::vec3 normal(1.f, 0.f, 0.f);
				bird->AddForce(2.f * glm::dot(bird->GetVelocity(), -normal) * (bird->GetMass() / dt) * normal);
			}
						
			break;
		}
	}
}

/*void Bird::StepBack(float _step, const std::vector<Bird*>& birds)
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
}*/

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
	if ((player->GetPosition().y > m_Position.y && (dt - LastFlap > 0.25f)) || (dt - LastFlap > 0.5f))
	{
		LastFlap = dt;
		return true;
	}

	return false;
}