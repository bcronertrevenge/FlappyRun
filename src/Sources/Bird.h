#pragma once

#include <glm/glm.hpp>
#include "Player.h"
#include <vector>

class Bird : public MovableObject
{
public:
	Bird(Player* _player, float posX);
	~Bird();

	void applyForce(glm::vec3 _pos);
	void Move(const std::vector<Bird*>& birds, float dt);
	void SetPosZ(float z);
	bool HasToFlap(float dt);

private:
	float speedIncrement;
	Player* player;
	float distanceMax;

	float LastFlap;
};

