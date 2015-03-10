#pragma once

#include <glm/glm.hpp>
#include "Player.h"
#include <vector>

class Bird : public MovableObject
{
public:
	Bird(Player* _player, float posX);
	~Bird();

	void StepBack(float _step, const std::vector<Bird*>& birds);
	void applyForce(glm::vec3 _pos);
	void Move(const std::vector<Bird*>& birds);
	void SetPosZ(float z);
	
private:
	float speedIncrement;
	Player* player;
	float distanceMax;

	bool isBirdDragged;
};

