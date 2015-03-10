#pragma once

#include <glm/glm.hpp>
#include "Player.h"

class Bird
{
public:
	Bird(Player* _player);
	~Bird();

	void StepBack(float _step);
	glm::vec3 GetPosition();
	void Move();
	float GetSize();
	bool CheckHitPlayer();
	void SetDragged(bool _drag = true);
	void SetPosZ(float z);

private:
	glm::vec3 Position;
	float sizeBird;
	float speedIncrement;
	Player* player;
	
	bool isBirdDragged;
};

